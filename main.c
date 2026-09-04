#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/history.h"
#include "includes/tokenizer.h"
#include "includes/var.h"
#include "includes/fs.h"

int main(){

    char line[1024];

    History history = init_strings();
    VariableTable var_table = init_var_table();
    
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        int line_length = strlen(line);
        if(line[line_length - 1] == '\n') line[line_length - 1] = '\0';

        char* cmds = expand_history(&history,line);
        TokenList token_list = tokenize(cmds);

        if(token_list.idx == 0) continue;

        token_list = expand_vars(&var_table,token_list);

        empty_strings(&token_list);
    }

    return 0;
}