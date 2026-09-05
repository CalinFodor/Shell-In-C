#include "../includes/var.h"

VariableTable init_var_table(){
    VariableTable var_table;
    var_table.idx = 0;
    return var_table;
}

void add_var_to_table(VariableTable* var_table,char* name,char *value){

    int n = var_table->idx;

    for(int i=0;i<n;i++){
        char* key = var_table->entries[i].name;
        bool removed = var_table->entries[i].removed;

        if(equal_strings(key,name) && !removed){
            var_table->entries[i].value = copy_string(value,strlen(value));
            return;
        }
    }

    var_table->entries[var_table->idx].name = copy_string(name,strlen(name));
    var_table->entries[var_table->idx].value = copy_string(value,strlen(value));
    var_table->entries[var_table->idx].removed = false;
    var_table->idx++;
}

char* get_var_value_from_table(VariableTable* var_table,char* name){
    for(int i=0;i<var_table->idx;i++){
        char* key = var_table->entries[i].name;
        char* key_value = var_table->entries[i].value;
        bool removed = var_table->entries[i].removed;
        if(strcmp(name,key) == 0 && !removed){
            return copy_string(key_value,strlen(key_value));
        }    
    }
    return NULL;
}

void remove_var_from_table(VariableTable* var_table,char* name){
    for(int i=0;i<var_table->idx;i++){
        char* key = var_table->entries[i].name;
        char* key_value = var_table->entries[i].value;
        if(strcmp(name,key) == 0){
            var_table->entries[i].removed = true;
        }    
    }
}

void print_table(VariableTable var_table){
    for(int i=0;i<var_table.idx;i++){
        if(!var_table.entries[i].removed)
            printf("[%s]=[%s]\n",var_table.entries[i].name,var_table.entries[i].value);
    }
}

void free_table(VariableTable* var_table){
    for(int i=0;i<var_table->idx;i++){
        free(var_table->entries[i].name);
        free(var_table->entries[i].value);
    }
    var_table->idx = 0;
}


char* expand_var(VariableTable* var_table,char* token){

    if(token[0] != '$'){
        return token;
    }

    int token_length = strlen(token);
    int var_name_start = 0;

    ExpandForm expand_form;

    //$var
    if(token[1] != '{'){
        expand_form = NoCurly;
    }else {
        char* dd_ptr = strchr(token,':');
        if(token[2] == '#'){
            expand_form = Length;
        }else if(dd_ptr!= NULL){
            int line_pos = dd_ptr - token + 1;
            if(token[line_pos] == '-')
                expand_form = DoubleDot;
        }else{
            expand_form = NormalCurly; 
        }
    }

    char* var_name = NULL;
    char* value = NULL;

    switch(expand_form){
        case NoCurly:
            var_name = copy_string(token+1,strlen(token)-1);

            value = get_var_value_from_table(var_table,var_name);
            break;
        case NormalCurly:
            var_name = copy_string(token+2,strlen(token)-2);
            var_name[strlen(var_name) - 1] = '\0';

            value = get_var_value_from_table(var_table,var_name);
            break;
        case DoubleDot:
            var_name = copy_string(token+2,strlen(token) - 2);
            int double_dot_pos = strchr(token,':') - token;
            var_name[double_dot_pos - 2] = '\0';

            value = get_var_value_from_table(var_table,var_name);

            if(value == NULL){
                int fallback_pos = double_dot_pos + 2;

                value = copy_string(token+fallback_pos,strlen(token)-2);
                value[strlen(value) - 1] = '\0'; //get rid of '}'
            }
            break;
        case Length:
            var_name = copy_string(token+3,strlen(token)-3);
            var_name[strlen(var_name) - 1] = '\0';

            char* val = get_var_value_from_table(var_table,var_name);

            if(val == NULL){
                value = malloc(2);
                value[0] = '0';
                value[1] = '\0';
            }else{
                int val_len = strlen(val);

                int num_len = get_num_len(val_len);
                value = malloc(num_len+1);
                sprintf(value,"%d",val_len);
            }
            break;
    }
    return value;
}

StringList expand_vars(VariableTable* var_table,StringList str_list){
    int n = str_list.idx;

    for(int i=0;i<n;i++){
        char* value = expand_var(var_table,str_list.elements[i]);
        if(value != NULL)
            str_list.elements[i] = value;
        else
            str_list.elements[i] = copy_string("",strlen(""));
    }
    return str_list;
}
