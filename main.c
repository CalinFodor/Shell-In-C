#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/history.h"

int main(){

    char line[1024];

    History history = init_history();
    
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        int line_length = strlen(line);
        if(line[line_length - 1] == '\n') line[line_length - 1] = '\0';

        char* proc_hist = expand_history(&history,line);
        printf("%s\n",proc_hist);
    }

    return 0;
}