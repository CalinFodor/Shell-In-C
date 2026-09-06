#ifndef VAR_H
#define VAR_H
#include "stringlib.h"

typedef struct _VarTabEntry{
    char* name;
    char* value;
    bool removed;
} VarTabEntry;

typedef struct _VariableTable{
    VarTabEntry entries[128];
    int idx;
} VariableTable;

typedef enum _ExpandForm{
    NoCurly,
    NormalCurly,
    DoubleDot,
    Length
}ExpandForm;

VariableTable init_var_table();
void add_var_to_table(VariableTable* var_table,char* name,char *value);
char* get_var_value_from_table(VariableTable* var_table,char* name);
void remove_var_from_table(VariableTable* var_table,char* name);
void print_table(VariableTable var_table);
void free_table(VariableTable* var_table);
TokenList expand_vars(VariableTable* var_table,TokenList str_list);

#endif