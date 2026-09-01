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

char* substring(char* string,int n){
    int len = strlen(string) - n;
    char* substr = (char*)malloc(len + 1);
    strncpy(substr,string + n,len);
    substr[len] = '\0';

    return substr;
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

bool is_empty(char* string){
    return strlen(string) == 0;
}

static int myCompare(const void* a, const void* b) 
{ 
    return strcmp(*(const char**)a, *(const char**)b); 
} 

void sort(char* arr[], int n) 
{ 
    qsort(arr, n, sizeof(char*), myCompare); 
} 

struct FS{
    char* files[128];
    int idx;
};

struct FS FILE_SYSTEM;

void add_file(char filename[]){
    add_string(FILE_SYSTEM.files,filename,FILE_SYSTEM.idx++);
}

bool set_match(char letter,char** pattern_ptr){
    char* pattern = *pattern_ptr;
    if(pattern[0] != '[')
        return false;
    
    int pattern_length = strlen(pattern);
    bool exclude = false;

    //pattern is [<chars>]

    if(pattern[1] == '!'){
       //exclude the following char
       exclude = true;
    }

    int brac_poz = strchr(pattern,']') - pattern;
    *pattern_ptr = substring(pattern,brac_poz);

    for(int i=0;i<brac_poz;i++){
        if(letter == pattern[i]){
            return !exclude;
        }
    }

    return exclude;
    
}

bool dot_check(char* pattern,char* name){
    if(is_empty(name) || is_empty(pattern))
        return true;
    
    if(pattern[0] == '*' && name[0] == '.')
        return false;
    return true;
}

bool match(char* pattern,char* name){

    if(is_empty(pattern))
        return is_empty(name);
    
    if(pattern[0] == '*'){
        return match(substring(pattern,1),name) 
            || (!is_empty(name) && match(pattern,substring(name,1)));
    }
    
    if(is_empty(name))
        return false;
    
    if(pattern[0] == '?' || pattern[0] == name[0] || set_match(name[0],&pattern)){
        return match(substring(pattern,1),substring(name,1));
    }
    return false;
}

void match_files(char* pattern){
    char* results[128];
    int res_idx = 0;

    for(int i=0;i<FILE_SYSTEM.idx;i++){
        char* name = FILE_SYSTEM.files[i];
        if(dot_check(pattern,name) && match(pattern,name)){
            add_string(results,name,res_idx++);
        }
    }

    if(res_idx == 0){
        printf("%s\n",pattern);
        return;
    }

    sort(results,res_idx);
    print_strings(results,res_idx);
        
}


void parse_command(char line[],char** cmd,char** argv){

    int space_poz = strchr(line,' ') - line;

    int cmd_len = space_poz;
    copy_string(cmd,line,space_poz);
    copy_string(argv,line + space_poz + 1,strlen(line) - space_poz);
}

void execute_command(char line[]){
    char* cmd = NULL;
    char* argv = NULL;

    parse_command(line,&cmd,&argv);
    
    if(cmd != NULL && argv != NULL){
        if(strcmp(cmd,"FILE") == 0){
            add_file(argv);
        }else if(strcmp(cmd,"MATCH") == 0){
            match_files(argv);
        }
        else{
            printf("ERROR: Unknown command\n");
        }

    }else{
        printf("ERROR: Could not parse\n");
    }
}

int main(void)
{

    FILE_SYSTEM.idx = 0;

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