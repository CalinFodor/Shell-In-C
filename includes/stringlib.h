#ifndef STRINGLIB_H
#define STRINGLIB_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _StringList{
    char* elements[64];
    int idx;
} StringList;


typedef struct{
    char** items;
    int count;
    int capacity;
}DA_StringList;

#define da_append(xs, x)                                                             \
    do {                                                                             \
        if ((xs)->count >= (xs)->capacity) {                                         \
            if ((xs)->capacity == 0) (xs)->capacity = 256;                           \
            else (xs)->capacity *= 2;                                                \
            (xs)->items = realloc((xs)->items, (xs)->capacity*sizeof(*(xs)->items)); \
        }                                                                            \
                                                                                     \
        (xs)->items[(xs)->count++] = (x);                                            \
    } while (0)


typedef DA_StringList TokenList;

StringList init_strings();
DA_StringList init_da_str(int capacity);
void free_da_str(DA_StringList* str_list);
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