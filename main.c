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

char* echo(char* text){
    return text;
}

char* upper(char* text){
    char* upper_text = copy_string(text,strlen(text));
    int s_len = strlen(upper_text);
    for(int i=0;i<s_len;i++){
        upper_text[i] = toupper(upper_text[i]);
    }
    return upper_text;
}

int get_num_len(int n){
    int i=0;
    while(n){
        i++;
        n = n / 10;
    }
    return i; 
}

char* len(char* text){
    int num = strlen(text);
    int num_len = get_num_len(num);
    char* res = (char*)malloc(num_len+1);

    sprintf(res,"%d",num);
    res[num_len] = '\0';
    return res;
}



int parse_var_ref(char argv[],int offset,char** value){
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
            var_end = i - 1;
            if(isalnum(character) && character != '}'){
                var_end++;
            }
            break;
        }
    }

    char* var_name = copy_string(argv+offset+1,var_end - offset);
    *value = get_var_value_from_table(var_name);
    
    int result = var_end + 1;
    if(curly_brace) result++;

    return result;

}

int parse_cmd_sub(char argv[],int offset,char** value){

    int argv_len = strlen(argv);
    int command_end = 0;

    char* commands[128];
    int commands_idx = 0;
    int paranthesis = 0;

    char* command_arg = NULL;

    int end = 0;

    int i=offset;
    int command_start = i;
    int arg_start = i;

    bool in_command = false;
    bool in_arg = false;

    //first symbol is $
    while(i<argv_len)
    {
        char character = argv[i];

        if(character == ' '){
            int command_len = i - command_start;
            commands[commands_idx++] = copy_string(argv+command_start,command_len);
            command_start = i+1;
            in_command = false;
        }else if(character == '$'){
            char next_character = argv[i+1];
            if(next_character != '('){
                //its an expandable variable if no (
                i = parse_var_ref(argv,i,&command_arg) - 1;
            }else{
                paranthesis++;
                command_start += 2;
                i++;
                in_command = true;
            }
        }else if(character == ')'){
            paranthesis--;
            if(in_arg){
                in_arg = false;
                command_arg = copy_string(argv+arg_start,i - arg_start);
            }

            if(paranthesis == 0){
                end = i;
                break;
            }
        }else if(!in_command && !in_arg){
            arg_start = i;
            in_arg = true;
        }
        i++;
    }

    //pop commands from the stack
    for(int i=commands_idx - 1;i>=0;i--){
        char* command = commands[i];

        if(strcmp(command,"echo") == 0){
            command_arg = echo(command_arg);
        }else if(strcmp(command,"upper") == 0){
            command_arg = upper(command_arg);
        }else if(strcmp(command,"len") == 0) {
            command_arg = len(command_arg);
        }else{
            command_arg = NULL;
        }
    }
    *value = command_arg;

    return end+1;
    
}


void expand(char argv[]){

    int argv_len = strlen(argv);
    int i=0;
    char* value = NULL;

    while(i<argv_len){
        char character = argv[i];
        if(character != '$'){
            printf("%c",character);
            i++;
        }else{
            if(i != argv_len - 1){
                char next_character = argv[i+1];
                if(next_character == '('){
                    i = parse_cmd_sub(argv,i,&value);
                }else{
                    i = parse_var_ref(argv,i,&value);
                }
                if(value != NULL){
                    printf("%s",value);
                    free(value);
                    value = NULL;
                }
            }
        }
    }
    printf("\n");

}

void execute_command(char line[]){
    char* cmd = NULL;
    int offset = get_one_token(line,&cmd) + 1;

    if(strcmp(cmd,"SET") == 0){
        set(line+offset);
        //print_table();
    }
    else if(strcmp(cmd,"EXPAND") == 0){
        expand(line+offset);
    }else{
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