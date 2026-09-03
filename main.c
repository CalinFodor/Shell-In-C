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

bool equal_strings(char* one,char* two){
    return (strcmp(one,two) == 0);
}

int tokenize(char line[],char* tokens[]){
    
    int token_idx = 0;

    int line_length = strlen(line);

    bool in_token = false;

    int token_start = 0;

    for(int i=0;i<line_length;i++){
        char character = line[i];
        if(character == ' ' && in_token){
            int token_len = i - token_start;
            tokens[token_idx++] = copy_string(line + token_start,token_len);
            in_token = false;
            token_start = i + 1;
        }else if(character == ' '){
            token_start++;
        }else if(!in_token){
            in_token = true;
        }
    }

    if(in_token){
        int token_len = line_length - token_start;
        tokens[token_idx++] = copy_string(line + token_start,token_len);
    }
    return token_idx;

}

typedef enum _State{
    Prompt,
    Running,
    Stopped,
}State;

State STATE = Prompt;

int fg_pid = 0;

void START(int pid){
    STATE = Running;
    fg_pid = pid;
    printf("started %d\n",pid);
}

void EXIT(int pid){
    if(pid == fg_pid){
        STATE = Prompt;
        fg_pid = 0;
        printf("done %d\n",pid);
    }
}

void STATUS(){
    char buf[20];
    switch(STATE){
        case Prompt:
            strcpy(buf,"prompt");
            break;
        case Running:
            strcpy(buf,"running");
            break;
        case Stopped:
            strcpy(buf,"stopped");
            break;
    }

    printf("state=%s fg=%d\n",buf,fg_pid);
}

void SIGINT(){
    if(STATE == Prompt){
        printf("^C\nprompt\n");
    }else if(STATE == Running){
        printf("forwarded SIGINT to %d\n",fg_pid);
    }
}

void SIGSTP(){
    if(STATE == Running){
        STATE = Stopped;
        printf("stopped %d\n",fg_pid);
    }else if(STATE == Prompt){
        printf("(no foreground job)\n");
    }
}

void SIGTERM(){
    printf("shell exiting\n");
    exit(0);
}

void execute_command(char line[]){
    char* tokens[128];
    int n = tokenize(line,tokens);

    char* cmd = tokens[0];

    if(equal_strings(cmd,"START")){
        int pid = atoi(tokens[1]);
        START(pid);
    }else if(equal_strings(cmd,"EXIT")){
        int pid = atoi(tokens[1]);
        EXIT(pid);
    }else if(equal_strings(cmd,"SIGINT")){
        SIGINT();
    }else if(equal_strings(cmd,"SIGTSTP")){
        SIGSTP();
    }else if(equal_strings(cmd,"SIGTERM")){
        SIGTERM();
    }else if(equal_strings(cmd,"STATUS")){
        STATUS();
    }
    else {
        printf("ERROR: Unknown command\n");
    }
}

int main(void)
{

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