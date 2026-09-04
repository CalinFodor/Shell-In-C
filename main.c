#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/history.h"
#include "includes/tokenizer.h"
#include "includes/var.h"
#include "includes/fs.h"
#include "includes/glob.h"
#include "includes/parser.h"
#include "includes/executor.h"

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
        token_list = expand_globs(token_list);

        add_command_to_hist(&history,line);

        CmdList cmd_list = parse_commands(token_list);
        run_commands(cmd_list.parsed_cmds[0].cmds,history);
        
        empty_strings(&token_list);
        for(int i=0;i<cmd_list.idx;i++){
            empty_strings(&cmd_list.parsed_cmds[i].cmds);
        }
    }

    return 0;
}