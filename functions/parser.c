#include "../includes/parser.h"


bool is_separator(char* s){
    return equal_strings(s,"&&") || equal_strings(s,"||") || equal_strings(s,";");
}

CmdList parse_commands(StringList tokens){

    CmdList cmd_list = {0};
    cmd_list.idx = 0;

    for(int i=0;i<tokens.idx;i++){
        char* token = tokens.elements[i];
        if(is_separator(token)){
            cmd_list.parsed_cmds[cmd_list.idx++].continuation = token;
        }else{
            add_string_to_list(&cmd_list.parsed_cmds[cmd_list.idx].cmds,token);
            if(i == tokens.idx - 1){
                cmd_list.parsed_cmds[cmd_list.idx++].continuation = NULL;
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
