#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

 
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

void copy_string(char **dest, char *src)
{
    int s_len = strlen(src) + 1;
    *dest = (char *)malloc(s_len);
    strncpy(*dest, src, s_len - 1);
    (*dest)[s_len - 1] = '\0';
}


int tokenize(char line[],char* tokens[],char separator){

    int token_index = 0;

    int token_start = 0;
    int in_token = 0;

    int line_length = strlen(line);

    for(int i=0;i<line_length;i++){
        char character = line[i];

        if(character == separator && in_token){
            int s_len = i - token_start + 1;
            
            tokens[token_index] = (char*)malloc(s_len);
            strncpy(tokens[token_index],line + token_start,s_len - 1);
            tokens[token_index++][s_len - 1] = '\0';

            in_token = 0;
            token_start = i;
            
        }else if(character != separator){
            if(!in_token){
                token_start = i;
                in_token = 1;
            }
        }

        if(in_token && i == line_length - 1){
            int len = line_length - token_start;

            tokens[token_index] = (char*)malloc(len+1);
            strncpy(tokens[token_index],line + token_start,len);
            tokens[token_index++][len] = '\0';
        }

    }
    return token_index;
}

void print_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++){
        printf("[%s] ",tokens[i]);
        free(tokens[i]);
    }
    printf("\n");
}

typedef struct _VarTabEntry{
    char* name;
    char* value;
} VarTabEntry;

typedef struct _VariableTable{
    VarTabEntry entries[128];
    int idx;
} VariableTable;

VariableTable VAR_TABLE;

void add_var_to_table(char* name,char *value){
    copy_string(&VAR_TABLE.entries[VAR_TABLE.idx].name,name);
    copy_string(&VAR_TABLE.entries[VAR_TABLE.idx++].value,value);
}

void print_table(){
    for(int i=0;i<VAR_TABLE.idx;i++){
        printf("[%s] -> [%s]\n",VAR_TABLE.entries[i].name,VAR_TABLE.entries[i].value);
    }
}

void free_table(){
    for(int i=0;i<VAR_TABLE.idx;i++){
        free(VAR_TABLE.entries[i].name);
        free(VAR_TABLE.entries[i].value);
    }
}

void execute_command(char* tokens[],int n){
    char* cmd = tokens[0];

    if(strcmp(cmd,"SET") == 0){
        if(n != 3){
            printf("ERROR: Invalid arguments for SET\n");
            return;
        }

        char* name = tokens[1];
        char* value = tokens[2];

        add_var_to_table(name,value);
    }else{
        printf("ERROR: Unknown command\n");
    }
}

void parse_and_execute_command(char line[]){
    char* tokens[128];
    int n = tokenize(line,tokens,' ');
    execute_command(tokens,n);
    print_table();
}


int main(void)
{
    VAR_TABLE.idx = 0;

    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       int line_length = strlen(line);
       if(line[line_length - 1] == '\n'){
            line[line_length - 1] = '\0';
       }
       if(line[0] == '\n') continue;
       parse_and_execute_command(line);
    }

    free_table();

    return 0;
}