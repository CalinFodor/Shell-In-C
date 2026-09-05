#include "../includes/parser.h"

bool is_redirect_operator(char *token)
{
    if (strlen(token) == 1)
        return token[0] == '<' || token[0] == '>';
    if (strlen(token) == 2)
        return (isdigit(token[0]) && token[1] == '>') || (token[0] == '>' && token[1] == '>');
    return false;
}

bool is_separator(char *s)
{
    return equal_strings(s, "&&") || equal_strings(s, "||") || equal_strings(s, ";");
}

RedirInfo parse_redir_operator(char *redir_operator, char *targe_file)
{
    RedirInfo redir_info = {0};
    redir_info.target_file = copy_string(targe_file, strlen(targe_file));

    int arrow_start = 0;
    int op_len = strlen(redir_operator);

    bool custom_fd = false;
    if (isdigit(redir_operator[0]))
    {
        redir_info.fd = redir_operator[0] - '0';
        arrow_start = 1;
        custom_fd = true;
    }

    if (redir_operator[arrow_start] == '>')
    {
        redir_info.redir_dir = Output;
        if (redir_operator[arrow_start + 1] == '>')
        {
            redir_info.modify_flag = Append;
        }
        else
        {
            redir_info.modify_flag = Trunc;
        }

        if (!custom_fd)
        {
            redir_info.fd = 1;
        }
    }
    else if (redir_operator[arrow_start] == '<')
    {
        redir_info.redir_dir = Input;

        if (!custom_fd)
        {
            redir_info.fd = 0;
        }
    }

    return redir_info;
}

bool is_pipe(char *s)
{
    return (strcmp(s, "|") == 0);
}

CmdList parse_commands(StringList tokens)
{
    CmdList cmd_list = {0};
    cmd_list.idx = 0;

    CmdPipeline buf_pipeline = {0};

    for (int i = 0; i < tokens.idx; i++)
    {
        char *token = tokens.elements[i];

        if (is_separator(token) || i == tokens.idx - 1)
        {
            if (!is_separator(token)){
                buf_pipeline.continuation = NULL;
                add_string_to_list(&buf_pipeline.parsed_cmd[buf_pipeline.idx].args,token);
            }
            else{
                buf_pipeline.continuation = copy_string(token, strlen(token));
            }

            buf_pipeline.idx++;
            cmd_list.pipelines[cmd_list.idx++] = buf_pipeline;
            buf_pipeline.idx = 0;

            if (buf_pipeline.continuation != NULL)
                free(buf_pipeline.continuation);
        }
        else if (is_pipe(token))
        {
            buf_pipeline.idx++;
        }
        else if (is_redirect_operator(token))
        {
            if (i != tokens.idx - 1)
            {
                char *target_file = tokens.elements[i + 1];
                buf_pipeline.parsed_cmd[buf_pipeline.idx++].redir_info = parse_redir_operator(token, target_file);
                i++;
                
                if(i == tokens.idx - 1){
                    cmd_list.pipelines[cmd_list.idx++] = buf_pipeline;
                    buf_pipeline.idx = 0;
                }
            }
        }else {
            add_string_to_list(&buf_pipeline.parsed_cmd[buf_pipeline.idx].args,token);
        }
    }
    return cmd_list;
}



