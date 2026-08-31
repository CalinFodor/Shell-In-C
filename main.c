#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

 
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

void copy_string(char **dest, char *src)
{
    int s_len = strlen(src) + 1;
    *dest = (char *)malloc(s_len);
    strncpy(*dest, src, s_len - 1);
    (*dest)[s_len - 1] = '\0';
}

typedef enum _Redir{
    None,
    Arrow,
    LineArrow
}Redir;

Redir parse_cmd(char* cmd,int* term_idx){

    int cmd_len = strlen(cmd);
    Redir redir = None;

    for(int i=0;i<cmd_len;i++){
        char character = cmd[i];

        if(character == '<' && i != cmd_len - 1){
            char next_character = cmd[i+1];
            if(next_character == '<'){
                char next_next_character = cmd[i+2];

                if(next_next_character == '-'){
                    redir = LineArrow;
                    *term_idx = i + 3;
                }else{
                    redir = Arrow;
                    *term_idx = i+2;
                }
                break;
            }
        }
    }
    return redir;

}

void extract_terminator(char* line,int term_idx,char term[]){
    int s_len = strlen(line);
    int found_sep = 0;
    for(int i=term_idx;i<s_len;i++){
        char character = line[i];

        if(character == '\'' || character == '"'){
            if(found_sep){
                break;
            }
            found_sep = 1;
        }else  {
            add_char(term,character);
        }
    }
}

void trim_leading_tabs(char* s){
    int i=0,j=0;

    while(s[i] == '\t') i++;

    while(s[j++] = s[i++]);

}

void parse_here_docs(char* lines[],int n){

    Redir redir = None;
    char term[128] = {'\0'};
    int term_idx = 0;

    for(int i=0;i<n;i++){
        char* line = lines[i];
        
        if(redir == None){
            redir = parse_cmd(line,&term_idx);
            printf("CMD %s\n",line);
            printf("BODY:\n");
        }
        
        switch(redir){
            case None:    
                printf("END\n");
                break;
            case Arrow:
                if(strlen(term) == 0){
                    extract_terminator(line,term_idx,term);
                    continue;
                }

                if(strcmp(term,line) == 0){
                    printf("END\n");
                    redir = None;
                }else{
                    printf("%s\n",line);
                }
                
                break;
            case LineArrow:
                if(strlen(term) == 0){
                    extract_terminator(line,term_idx,term);
                    trim_leading_tabs(term);
                    continue;
                }

                if(strcmp(term,line) == 0){
                    printf("END\n");
                    redir = None;
                }else{
                    trim_leading_tabs(line);
                    printf("%s\n",line);
                }

                break;
        }
    }

}

void free_strings(char* strings[],int n){
    for(int i=0;i<n;i++)
        free(strings[i]);
}

int main(void)
{
    char line[1024];
    char *lines[1024];
    int nolines = 0;

    Redir redir = None;
    char term[128] = {'\0'};
    int term_idx = 0;

    while (fgets(line, sizeof line, stdin))
    {
       if(line[strlen(line) - 1] == '\n') 
            line[strlen(line) - 1] = '\0';
        
       if(redir == None){
            redir = parse_cmd(line,&term_idx);
            printf("CMD %s\n",line);
            printf("BODY:\n");
        }
        
        switch(redir){
            case None:    
                printf("END\n");
                break;
            case Arrow:
                if(strlen(term) == 0){
                    extract_terminator(line,term_idx,term);
                    continue;
                }

                if(strcmp(term,line) == 0){
                    printf("END\n");
                    redir = None;
                }else{
                    printf("%s\n",line);
                }
                
                break;
            case LineArrow:
                if(strlen(term) == 0){
                    extract_terminator(line,term_idx,term);
                    trim_leading_tabs(term);
                    continue;
                }
                trim_leading_tabs(line);

                if(strcmp(term,line) == 0){
                    printf("END\n");
                    redir = None;
                }else{
                    printf("%s\n",line);
                }

                break;
        }
    }

    return 0;
}