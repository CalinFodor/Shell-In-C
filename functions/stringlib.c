#include "../includes/stringlib.h"

StringList init_strings(){
    StringList s_list;
    s_list.idx = 0;
    return s_list;
}


void add_char(char *s, char c)
{
    int l = strlen(s);
    s[l] = c;
    s[l + 1] = '\0';
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

void add_string_to_list(StringList* s_list, char *string)
{
    int s_len = strlen(string) + 1;
    int poz = s_list->idx;

    s_list->elements[poz] = (char *)malloc(s_len);
    strncpy(s_list->elements[poz], string, s_len - 1);
    s_list->elements[poz][s_len - 1] = '\0';

    s_list->idx++;
}

bool starts_with(char *str, char *pre)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_number(char* str){
    int s_len = strlen(str);
    for(int i=0;i<s_len;i++){
        if(!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

void print_strings(StringList str_list){
    int n = str_list.idx;
    for(int i=0;i<n;i++){
        printf("%s\n",str_list.elements[i]);
    }
}

void empty_strings(StringList* str_list){
    int n = str_list->idx;
    for(int i=0;i<n;i++){
        free(str_list->elements[i]);
    }

    str_list->idx = 0;
}

int get_num_len(int n){
    int l = 0;
    while(n){
        n = n/10;
        l++;
    }
    return l;
}

bool is_empty(char* string){
    return strlen(string) == 0;
}

static int stringCompare(const void* a, const void* b) 
{ 
    return strcmp(*(const char**)a, *(const char**)b); 
} 

void sort(char* arr[], int n) 
{ 
    qsort(arr, n, sizeof(char*), stringCompare); 
} 

char* substring(char* string,int n){
    int len = strlen(string) - n;
    char* substr = (char*)malloc(len + 1);
    strncpy(substr,string + n,len);
    substr[len] = '\0';

    return substr;
}

