#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
 
void add_char(char *s, char c)
{
    int l = strlen(s);
    s[l] = c;
    s[l + 1] = '\0';
}

void add_string(char *strings[], char *string, int poz)
{
    int s_len = strlen(string) + 1;
    strings[poz] = (char *)malloc(s_len);
    strncpy(strings[poz], string, s_len - 1);
    strings[poz][s_len - 1] = '\0';
}

char* copy_string(char *src,int len)
{
    char* dest = NULL;
    dest = (char *)malloc(len+1);
    strncpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

bool equal_strings(char* one,char* two){
    return (strcmp(one,two) == 0);
}

int tokenize(char line[],char* tokens[]){
    
    int token_idx = 0;

    int line_length = strlen(line);

    bool in_token = false;

    int token_start = 0;

    for(int i=0;i<line_length;i++){
        char character = line[i];
        if(character == ' ' && in_token){
            int token_len = i - token_start;
            tokens[token_idx++] = copy_string(line + token_start,token_len);
            in_token = false;
            token_start = i + 1;
        }else if(character == ' '){
            token_start++;
        }else if(!in_token){
            in_token = true;
        }
    }

    if(in_token){
        int token_len = line_length - token_start;
        tokens[token_idx++] = copy_string(line + token_start,token_len);
    }
    return token_idx;

}

typedef enum _ProcStatus{
    Running,
    Zombie,
    Reaped
} ProcStatus;

typedef struct _ProcTableEntry{
    int pid;
    int parent_pid;
    ProcStatus status;
    char* program;
    int exit_code;
} ProcTableEntry;


typedef struct _ProcTable{
    ProcTableEntry table[128];
    int i;
} ProcTable;

ProcTable PROC_TABLE;

void FORK(int parent_pid,int pid){
    ProcTableEntry new_entry;

    bool found_parent = false;
    int entries = PROC_TABLE.i;
    for(int i=0;i<entries;i++){
        ProcTableEntry entry = PROC_TABLE.table[i];
        if(entry.pid == parent_pid){
            found_parent = true;
            new_entry.program = copy_string(entry.program,strlen(entry.program));
        }
    }

    if(!found_parent){
        printf("FORK: Could not find parent\n");
        return;
    }

    new_entry.parent_pid = parent_pid;
    new_entry.pid = pid;
    new_entry.status = Running;
    new_entry.exit_code = -1;

    PROC_TABLE.table[PROC_TABLE.i++] = new_entry;
}

void EXEC(int pid,char* prog){

    int entries = PROC_TABLE.i;
    for(int i=0;i<entries;i++){
        ProcTableEntry entry = PROC_TABLE.table[i];
        if(pid == entry.pid){
            PROC_TABLE.table[i].program = copy_string(prog,strlen(prog));
            return;
        }
    }

    printf("EXEC: Could not find pid\n");
}

void EXIT(int pid,int code){
    int entries = PROC_TABLE.i;
    for(int i=0;i<entries;i++){
        ProcTableEntry entry = PROC_TABLE.table[i];
        if(pid == entry.pid){
            PROC_TABLE.table[i].exit_code = code;
            PROC_TABLE.table[i].status = Zombie;
            return;
        }
    }
    printf("EXIT: Could not find pid\n");
}

void WAIT(int pid,int child){
    int entries = PROC_TABLE.i;
    for(int i=0;i<entries;i++){
        ProcTableEntry entry = PROC_TABLE.table[i];
        if(pid == entry.parent_pid && child == entry.pid){
            if(entry.status == Zombie){
                PROC_TABLE.table[i].status = Reaped;
                printf("%d\n",entry.exit_code);
                return;
            }
        }
    }
    printf("-1\n");
}

void STATUS(int pid){
    int entries = PROC_TABLE.i;
    for(int i=0;i<entries;i++){
        ProcTableEntry entry = PROC_TABLE.table[i];
        if(pid == entry.pid){
            char status[128] = {'\0'};

            switch(entry.status){
                case Running:
                    strcpy(status,"running");
                    break;
                case Zombie:
                    strcpy(status,"zombie");
                    break;
                case Reaped:
                    strcpy(status,"reaped");
                    break;
            }

            printf("%s prog=%s\n",status,entry.program);
            return;
        }
    }
    printf("unknown prog=?\n");
}

void execute_command(char line[]){
   
    char* tokens[128];
    int n_tokens = tokenize(line,tokens);

    if(n_tokens < 1){
        return;
    }

    char* cmd = tokens[0];

    if(cmd != NULL){
        if(equal_strings(cmd,"FORK")){
            if(n_tokens >= 3){
                int parent = atoi(tokens[1]);
                int child = atoi(tokens[2]);
                FORK(parent,child);
            }
        }else if(equal_strings(cmd,"EXEC")){
            if(n_tokens >= 3){
                int pid = atoi(tokens[1]);
                char* prog = tokens[2];
                EXEC(pid,prog);
            }
        }else if(equal_strings(cmd,"EXIT")){
            if(n_tokens >= 3){
                int pid = atoi(tokens[1]);
                int code = atoi(tokens[2]);
                EXIT(pid,code);
            }
            
        }else if(equal_strings(cmd,"WAIT")){
            if(n_tokens >= 3){
                int pid = atoi(tokens[1]);
                int child = atoi(tokens[2]);
                WAIT(pid,child);
            }

        }else if(equal_strings(cmd,"STATUS")){
            if(n_tokens >= 2){
                int pid = atoi(tokens[1]);
                STATUS(pid);
            }
        }else{
            printf("ERROR: Unknown command\n");
        }
    }

}


void init_table(){
    PROC_TABLE.i = 0;

    char program_name[] = "shell";

    ProcTableEntry entry;
    entry.exit_code = -1;
    entry.parent_pid = -1;
    entry.pid = 0;
    entry.status = Running;

    entry.program = copy_string(program_name,strlen(program_name));

    PROC_TABLE.table[PROC_TABLE.i++] = entry;
}

int main(void)
{
    init_table();

    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       if(line[0] == '\n') continue; 

       int line_length = strlen(line);
       if(line[line_length - 1] == '\n'){
            line[line_length - 1] = '\0';
       }
       if(line[0] == '\n') continue;

       execute_command(line);
    }

    return 0;
}