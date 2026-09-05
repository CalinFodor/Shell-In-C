#include "../includes/parser.h"

bool is_redirect_operator(char* token){
    if(strlen(token) == 1)
        return token[0] == '<' || token[0] == '>';
    if(strlen(token) == 2)
        return (isdigit(token[0]) && token[1] == '>')
            || (token[0] == '>' && token[1] == '>');
    return false; 
}

RedirInfo parse_redir_operator(char* redir_operator,char* targe_file){
    RedirInfo redir_info = {0};
    redir_info.target_file = copy_string(targe_file,strlen(targe_file));

    int arrow_start = 0;
    int op_len = strlen(redir_operator);

    bool custom_fd = false;
    if(isdigit(redir_operator[0])){
        redir_info.fd = redir_operator[0] - '0';
        arrow_start = 1;
        custom_fd = true;
    }

    if(redir_operator[arrow_start] == '>'){
        redir_info.redir_dir = Output;
        if(redir_operator[arrow_start + 1] == '>'){
            redir_info.modify_flag = Append;
        }else{
            redir_info.modify_flag = Trunc;
        }

        if(!custom_fd){
            redir_info.fd = 1;
        }
    }else if(redir_operator[arrow_start] == '<'){
        redir_info.redir_dir = Input;

        if(!custom_fd){
            redir_info.fd = 0;
        }
    }

    return redir_info;    

}

bool is_separator(char* s){
    return equal_strings(s,"&&") || equal_strings(s,"||") || equal_strings(s,";");
}

CmdList parse_commands(StringList tokens){

    CmdList cmd_list = {0};
    cmd_list.idx = 0;

    for(int i=0;i<tokens.idx;i++){
        char* token = tokens.elements[i];
        if(is_separator(token)){
            cmd_list.cmd_info[cmd_list.idx++].continuation = token;
        }else if(is_redirect_operator(token)){
            if(i != tokens.idx - 1){
                char* target_file = tokens.elements[i+1];
                RedirInfo redir_info = parse_redir_operator(token,target_file);

                cmd_list.cmd_info[cmd_list.idx].redir_info = redir_info;
                //skip the target file 
                i++;
            }
        }
        else{
            add_string_to_list(&cmd_list.cmd_info[cmd_list.idx].args,token);
            if(i == tokens.idx - 1){
                cmd_list.cmd_info[cmd_list.idx++].continuation = NULL;
            }
        }
    }

    return cmd_list;
}

bool is_pipe(char* s){
    return (strcmp(s,"|") == 0);
}

int count_pipes(StringList tokens){
    int count = 0;
    for(int i=0;i<tokens.idx;i++){
        if(is_pipe(tokens.elements[i])){
            count++;
        }
    }
    return count;
}


void pipe_plan(StringList tokens){
    
    int pipe_count = count_pipes(tokens);

    int n = 2*pipe_count-1;

    int cmd_i = 0;
    bool in_command = false;
    for(int i=0;i<tokens.idx;i++){
        char* token = tokens.elements[i];
        if(!in_command && !is_pipe(token)){
            printf("CMD %d %s\n",cmd_i,token);
            in_command = true;
            if(pipe_count == 0){
                break;
            }
        }

        if(is_pipe(token) || i == tokens.idx - 1){
            int in_dup2 = 10 + 2*(cmd_i - 1);
            int out_dup2 = 11 + 2*cmd_i;

            //first pipe not dup2 stdin
            if(cmd_i != 0){
                printf("  DUP2 %d 0\n",in_dup2);
            }

            //last pipe not dup2 stdout
            if(cmd_i != pipe_count){
                printf("  DUP2 %d 1\n",out_dup2);
            }
            
          
            for(int i=0;i<=n;i++){
                printf("  CLOSE %d\n",10+i);
            }
            cmd_i++;
            in_command = false;
        }
    }
}
