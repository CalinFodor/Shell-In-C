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

char* substring(char* string,int n){
    int len = strlen(string) - n;
    char* substr = (char*)malloc(len + 1);
    strncpy(substr,string + n,len);
    substr[len] = '\0';

    return substr;
}

int get_one_token(char line[],char** token){
    char* space_ptr = strchr(line,' ');

    if(space_ptr == NULL) {
        *token = copy_string(line,strlen(line));
        return 0;
    }

    int space_poz = space_ptr - line;

    *token = copy_string(line,space_poz);
    return space_poz;

} 

void print_strings(char* strings[],int n){
    for(int i=0;i<n;i++){
        printf("%s",strings[i]);
        if(i != n - 1){
            printf(" ");
        }
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
    VAR_TABLE.entries[VAR_TABLE.idx].name = copy_string(name,strlen(name));
    VAR_TABLE.entries[VAR_TABLE.idx++].value = copy_string(value,strlen(value));
}

char* get_var_value_from_table(char* name){
    char* value = NULL;
    for(int i=0;i<VAR_TABLE.idx;i++){
        char* key = VAR_TABLE.entries[i].name;
        char* key_value = VAR_TABLE.entries[i].value;
        if(strcmp(name,key) == 0){
            value = copy_string(key_value,strlen(key_value));
            break;
        }    
    }
    return value;
}

void print_table(){
    for(int i=0;i<VAR_TABLE.idx;i++){
        printf("[%s] -> [%s]\n",VAR_TABLE.entries[i].name,VAR_TABLE.entries[i].value);
    }
}

void set(char argv[]){
    char* name = NULL;
    char* value = NULL;

    int offset = get_one_token(argv,&name) + 1;
    get_one_token(argv + offset,&value);

    add_var_to_table(name,value);
}

int parse_cmd_sub(char argv[],int offset){

}

int parse_var_ref(char argv[],int offset){
    bool curly_brace = false;
    if(argv[1] == '{'){
        offset++;
        curly_brace = true;
    }

    int var_end = 0;
    int argv_len = strlen(argv);
    for(int i=offset + 1;i<argv_len;i++){
        char character = argv[i];
        if((curly_brace && character == '}') || !isalnum(character) || i == strlen(argv) - 1){
            var_end = i;
            break;
        }
    }

    char* var_name = copy_string(argv+offset,var_end - offset);
    char* value = get_var_value_from_table(var_name);

    if(value != NULL)
        printf("%s",value);
    
    return var_end;

}

void expand(char argv[]){

    int argv_len = strlen(argv);
    int i=0;
    
    while(i<argv_len){
        char character = argv[i];
        if(character != '$'){
            printf("%c",character);
            i++;
        }else{
            if(i != argv_len - 1){
                char next_character = argv[i];
                if(next_character == '('){
                    i = parse_cmd_sub(argv,i);
                }else{
                    i = parse_var_ref(argv,i);
                }
            }
        }

    }

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
    }
    else{
        printf("ERROR: Unknown command\n");
    }
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

       execute_command(line);
    }

    return 0;
}