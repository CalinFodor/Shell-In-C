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

void add_token_to_pipeline(CmdList* pipeline,char* token){

    int pipe_count = pipeline->pipe_count;
    int cmd_count = pipeline->pipelines[pipe_count].cmd_count;
    TokenList* args = &pipeline->pipelines[pipe_count].parsed_cmd[cmd_count].args;
    da_append(args,token);
}

CmdList parse_commands(TokenList tokens)
{
    CmdList pipeline_list = {0};

    for (int i = 0; i < tokens.count; i++)
    {
        char *token = tokens.items[i];

        if (is_separator(token) || i == tokens.count - 1)
        {
            // if its a separator or end of tokens add it to the pipeline and end it
            if (!is_separator(token))
            {
                add_token_to_pipeline(&pipeline_list,token);
            }else{
                char* separator = token;
                pipeline_list.pipelines[pipeline_list.pipe_count].continuation = copy_string(separator,strlen(separator));
            }


            pipeline_list.pipelines[pipeline_list.pipe_count].cmd_count++;
            pipeline_list.pipe_count++;
        }
        else if (is_pipe(token))
        {    
            pipeline_list.pipelines[pipeline_list.pipe_count].cmd_count++;
        }
        else if (is_redirect_operator(token))
        {
            if (i != tokens.count - 1)
            {
                char* redir_oper = token;
                char* target_file = tokens.items[i+1];

                int pipe_count = pipeline_list.pipe_count;
                int cmd_count = pipeline_list.pipelines[pipe_count].cmd_count; 

                pipeline_list.pipelines[pipe_count].parsed_cmd[cmd_count].redir_info = parse_redir_operator(redir_oper,target_file);
                i++;

                if(i == tokens.count - 1){
                    pipeline_list.pipelines[pipeline_list.pipe_count].cmd_count++;
                    pipeline_list.pipe_count++;
                }
            }
        }
        else
        {
            int pipe_count = pipeline_list.pipe_count;
            int cmd_count = pipeline_list.pipelines[pipe_count].cmd_count; 
            da_append(&pipeline_list.pipelines[pipe_count].parsed_cmd[cmd_count].args,token);
        }
    }
    return pipeline_list;
}
