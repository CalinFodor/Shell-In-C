#ifndef STRINGLIB_H
#define STRINGLIB_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _StringList{
    char* elements[128];
    int idx;
} StringList;

StringList init_strings();
void add_char(char *s, char c);
char* copy_string(char *src,int len);
bool equal_strings(char* one,char* two);
void add_string_to_list(StringList* s_list, char *string);
bool starts_with(char *str, char *pre);
bool is_number(char* str);
void print_strings(StringList str_list);
void empty_strings(StringList* str_list);
int get_num_len(int n);
void sort(char* arr[], int n);
char* substring(char* string,int n);
bool is_empty(char* string);

#endif