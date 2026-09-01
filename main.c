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

void copy_string(char **dest, char *src,int len)
{
    *dest = (char *)malloc(len+1);
    strncpy(*dest, src, len);
    (*dest)[len] = '\0';
}

int get_one_token(char line[],char** token){
    char* space_ptr = strchr(line,' ');

    if(space_ptr == NULL) {
        copy_string(token,line,strlen(line));
        return 0;
    }

    int space_poz = space_ptr - line;

    copy_string(token,line,space_poz);
    return space_poz;

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
    copy_string(&VAR_TABLE.entries[VAR_TABLE.idx].name,name,strlen(name));
    copy_string(&VAR_TABLE.entries[VAR_TABLE.idx++].value,value,strlen(value));
}

void get_var_value_from_table(char* name,char** value){
    for(int i=0;i<VAR_TABLE.idx;i++){
        char* key = VAR_TABLE.entries[i].name;
        char* key_value = VAR_TABLE.entries[i].value;
        if(strcmp(name,key) == 0){
            copy_string(value,key_value,strlen(key_value));
        }    
    }
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

void set(char argv[]){
    char* name = NULL;
    char* value = NULL;

    int offset = get_one_token(argv,&name) + 1;
    get_one_token(argv + offset,&value);

    add_var_to_table(name,value);
}

typedef enum _ExpandForm{
    NoCurly,
    NormalCurly,
    DoubleDot,
    Length
}ExpandForm;

void expand_token(char* token){
    int token_length = strlen(token);
    int var_name_start = 0;

    ExpandForm expand_form;

    //$var
    if(token[1] != '{'){
        expand_form = NoCurly;
    }else {
        if(token[2] == '#'){
            expand_form = Length;
        }else if(strchr(token,':') != NULL){
            expand_form = DoubleDot;
        }else{
            expand_form = NormalCurly; 
        }
    }

    char* var_name;
    char* value = NULL;

    switch(expand_form){
        case NoCurly:
            copy_string(&var_name,token+1,strlen(token)-1);

            get_var_value_from_table(var_name,&value);

            if(value != NULL){
                printf("%s",value);
            }else{
                printf(" ");
            }
            break;
        case NormalCurly:
            copy_string(&var_name,token+2,strlen(token)-2);
            var_name[strlen(var_name) - 1] = '\0';

            get_var_value_from_table(var_name,&value);

            if(value != NULL){
                printf("%s",value);
            }else{
                printf(" ");
            }    
            break;
        case DoubleDot:
            copy_string(&var_name,token+2,strlen(token) - 2);
            int double_dot_pos = strchr(token,':') - token;
            var_name[strlen(var_name) - double_dot_pos - 1] = '\0';

            get_var_value_from_table(var_name,&value);

            char* fallback;

            int fallback_pos = double_dot_pos + 2;

            copy_string(&fallback,token+fallback_pos,strlen(token)-2);
            fallback[strlen(fallback) - 1] = '\0'; //get rid of '}'

            if(value != NULL){
                printf("%s",value);
            }else{
                printf("%s",fallback);
            }
            break;
        case Length:
            copy_string(&var_name,token+3,strlen(token)-3);
            var_name[strlen(var_name) - 1] = '\0';

            get_var_value_from_table(var_name,&value);

            int val_len = strlen(value);

            if(val_len){
                printf("%d",val_len);
            }else{
                printf("0 ");
            }
            break;
    }
}

void expand(char argv[]){
    char* token = NULL;
    int in_token = 0;
    int token_start = 0;
    int curly_brace = 0;

    int argv_len = strlen(argv);
    for(int i=0;i<argv_len;i++){
        char character = argv[i];

        if(character == '$'){
            in_token = 1;
            if(i != argv_len - 2){
                char next_character = argv[i+1];
                if(next_character == '{'){
                    curly_brace = 1;
                    i++;
                }
            }

        }else if( (curly_brace && character == '}') || (in_token && !isalpha(character) && !curly_brace) || (in_token && i == argv_len -1)){
            int token_len = i - token_start;

            if(curly_brace){
                token_len++;
            }

            copy_string(&token,argv + token_start,token_len);
            expand_token(token);
            token[0] = '\0';

            free(token);
            token = NULL;

            curly_brace = 0;
            in_token = 0;

            if(!isalpha(character) && character != '}'){
                printf("%c",character);
            }

        }else if(!in_token){
            token_start++;
            printf("%c",character);
        }
        
    }
    printf("\n");

}

void execute_command(char line[]){
    char* cmd = NULL;
    int offset = get_one_token(line,&cmd) + 1;

    if(strcmp(cmd,"SET") == 0){
        set(line+offset);
        print_table();
    }
    else if(strcmp(cmd,"EXPAND") == 0){
        expand(line+offset);
    }else{
        printf("ERROR: Unknown command\n");
    }
}

void parse_and_execute_command(char line[]){
    execute_command(line);
}


int main(void)
{
    VAR_TABLE.idx = 0;

    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       if(line[0] == '\n') continue; 

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