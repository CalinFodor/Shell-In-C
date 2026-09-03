#ifndef STRINGLIB_H
#define STRINGLIB_H
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _StringList{
    char* elements[128];
    int idx;
} StringList;

void add_char(char *s, char c);
char* copy_string(char *src,int len);
bool equal_strings(char* one,char* two);
void add_string_to_list(StringList* s_list, char *string);
bool starts_with(char *str, char *pre);
bool is_number(char* str);



#endif