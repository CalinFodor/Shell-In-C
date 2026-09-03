#include "../includes/history.h"
#include <stdio.h>

History init_history(){
    History history;
    history.idx = 0;
    return history;
}

void add_command_to_hist(History* history,char* command){
    add_string_to_list(history,command);
}

void print_hist(History history){
    int n = history.idx;
    for(int i=0;i<n;i++){
        printf("%d: %s\n",i+1,history.elements[i]);
    }
}

char* print_last_command(History* history){
    if(history -> idx == 0) return NULL;
    
    char* last_command = history->elements[history->idx - 1];
    add_command_to_hist(history,last_command);
    return last_command;
}

char* print_n_command(History* history,int n){
    int command_len = history->idx;
    for(int i=0;i<command_len;i++){
        if(i+1 == n){
            char *cmd = history->elements[i];
            add_command_to_hist(history,cmd);
            return cmd;
        }
    }
    printf("bash: !%d: event not found\n",n);
    return NULL;
}

char* print_match_command(History* history,char* str){
    int command_len = history->idx;

    for(int i=0;i<command_len;i++){
        char* cmd = history->elements[i];
        if(starts_with(cmd,str)){
            add_command_to_hist(history,cmd);
            return str;
        }
    }

    
    printf("bash: !%s: event not found\n",str);
    return NULL;
}

bool is_hist_command(char* cmd){
    return starts_with(cmd,"!") || starts_with(cmd,":");
}

char* expand_history(History* history,char line[]){
    char* cmd = NULL;

    if(!is_hist_command(line)){
        add_command_to_hist(history,line);
    }else{
        if(line[0] == '!'){
            if(line[1] == '!'){
               cmd = print_last_command(history);
            }else if(is_number(line + 1)){
                int n = atoi(line+1);
                cmd = print_n_command(history,n);
            }else{
                char* str = line + 1;
                cmd = print_match_command(history,str);
            }
        }else{
            print_hist(*history);
        }
    }
    
    if(cmd != NULL)
        return cmd;
    return line;
}
