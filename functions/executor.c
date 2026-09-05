#include "../includes/executor.h"

char OLDPWD[1024] = {'\0'};

static char *BUILTIN_CMDS[] = {"echo", "pwd", "cd", "exit", "history"};

bool is_builtin(char *cmd)
{
    int n = sizeof(BUILTIN_CMDS) / sizeof(char *);
    for (int i = 0; i < n; i++)
    {
        if (equal_strings(cmd, BUILTIN_CMDS[i]))
        {
            return true;
        }
    }
    return false;
}

int echo(StringList tokens)
{
    for (int i = 1; i < tokens.idx; i++)
        printf("%s ", tokens.elements[i]);
    printf("\n");
    return 0;
}

int pwd()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    if (strlen(cwd))
    {
        printf("%s\n", cwd);
    }
    return 0;
}

int cd(StringList tokens)
{

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    char *home = getenv("HOME");

    char *arg = NULL;
    if (tokens.idx == 2)
    {
        arg = tokens.elements[1];
    }

    if (tokens.idx == 1 || strcmp(arg, "~") == 0)
    {
        if (home == NULL)
        {
            printf("cd: HOME not set\n");
            return -1;
        }
        strcpy(OLDPWD, cwd);
        chdir(home);
    }
    else if (strcmp(arg, "-") == 0)
    {
        if (!strlen(OLDPWD))
        {
            printf("cd: OLDPWD not set\n");
            return -1;
        }
        else
        {
            chdir(OLDPWD);
            strcpy(OLDPWD, cwd);
        }
    }
    else
    {
        chdir(arg);
        strcpy(OLDPWD, cwd);
    }
    return 0;
}

int history(History history)
{
    print_hist(history);
    return 0;
}

int run_builtin_cmd(StringList tokens, History hist)
{
    char *cmd = tokens.elements[0];

    if (equal_strings(cmd, "echo"))
    {
        return echo(tokens);
    }
    else if (equal_strings(cmd, "pwd"))
    {
        return pwd();
    }
    else if (equal_strings(cmd, "cd"))
    {
        return cd(tokens);
    }
    else if (equal_strings(cmd, "exit"))
    {
        exit(0);
    }
    else if (equal_strings(cmd, "history"))
    {
        return history(hist);
    }
}

int apply_redirections(RedirInfo redir_info)
{
    char *file = redir_info.target_file;

    if (file == NULL)
        return -1;

    // dup2(src,des)
    int saved_fd = dup(redir_info.fd);
    if (file[0] == '&')
    {
        int old_fd = file[1] - '0';
        int new_fd = redir_info.fd;
        dup2(old_fd, new_fd);
        close(old_fd);
    }
    else
    {
        int fd;
        switch (redir_info.redir_dir)
        {
        case Input:
            fd = open(file, O_RDONLY, 0644);
            dup2(fd, redir_info.fd);
            break;
        case Output:

            if (redir_info.modify_flag == Trunc)
            {
                fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else
            {
                fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            dup2(fd, redir_info.fd);
            break;
        }
        if (fd < 0)
        {
            perror("open");
            return -1;
        }
        close(fd);
    }
    return saved_fd;
}

void restore_descriptor(RedirInfo redir_info, int saved_fd)
{
    dup2(saved_fd, redir_info.fd);
    close(saved_fd);
}

int run_command(ParsedCmd parsed_cmd, History history, int in, int out, int pipe_count)
{
    int saved_fd = apply_redirections(parsed_cmd.redir_info);

    StringList tokens = parsed_cmd.args;
    char *cmd = tokens.elements[0];

    if (is_builtin(cmd) && pipe_count == 0)
    {
        return run_builtin_cmd(tokens, history);
    }

    tokens.elements[tokens.idx++] = NULL;

    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        return 1;
    }
    if (child_pid == 0)
    {
        if (in != 0)
        {
            dup2(in, 0);
        }

        if (out != 1)
        {
            dup2(out, 1);
        }

        if (pipe_count != 0)
        {
            for (int i = BASE_FD; i <= BASE_FD + 2 * pipe_count; i++)
                close(i);
        }

        if (is_builtin(cmd))
        {
            run_builtin_cmd(tokens, history);
        }
        else
        {
            int status_code = execvp(cmd, tokens.elements);
        }

        perror(cmd);
    }
    else if(pipe_count == 0)
    {
        waitpid(child_pid, NULL, 0);
    }
    
    if (saved_fd != -1)
        restore_descriptor(parsed_cmd.redir_info, saved_fd);

    return 0;
}

int execute_pipeline(CmdPipeline pipeline, History history)
{
    if (pipeline.idx == 1)
    {
        return run_command(pipeline.parsed_cmd[0], history, 0, 1, 0);
    }

    int n = pipeline.idx;
    int pipe_count = n - 1;

    for (int i = 0; i < pipe_count; i++)
    {
        int temp_pipe[2];

        int target_in_fd = BASE_FD + 2 * i;
        int target_out_fd = BASE_FD + 1 + 2 * i;

        if (pipe(temp_pipe) < 0)
        {
            perror("pipe");
        }

        dup2(temp_pipe[0], target_in_fd);
        close(temp_pipe[0]);

        dup2(temp_pipe[1], target_out_fd);
        close(temp_pipe[1]);
    }

    int exit_code;

    for (int i = 0; i < n; i++)
    {
        ParsedCmd parsed_cmd = pipeline.parsed_cmd[i];

        int in_dup2 = 0;
        int out_dup2 = 1;

        if (i > 0)
            in_dup2 = BASE_FD + 2 * (i - 1);

        if (i < n - 1)
            out_dup2 = BASE_FD + 1 + 2 * i;

        exit_code = run_command(parsed_cmd, history, in_dup2, out_dup2, pipe_count);
        empty_strings(&parsed_cmd.args);
    }

    for (int i = BASE_FD; i < BASE_FD + 2 * pipe_count; i++)
        close(i);

    for (int i = 0; i < n; i++)
        wait(NULL);

    return exit_code;
}