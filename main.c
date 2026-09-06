#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/ui.h"
#include "includes/history.h"
#include "includes/tokenizer.h"
#include "includes/var.h"
#include "includes/fs.h"
#include "includes/glob.h"
#include "includes/parser.h"
#include "includes/executor.h"


int main(){

    char line[1024];

    History history = init_history();
    VariableTable var_table = init_var_table();

    print_prompt();

    while (fgets(line, sizeof line, stdin)) {
        
        if (line[0] == '\n' || line[0] == 0) continue;
        int line_length = strlen(line);
        if(line[line_length - 1] == '\n') line[line_length - 1] = '\0';

        char* cmds = expand_history(&history,line);
        TokenList token_list = tokenize(cmds);

        if(token_list.count == 0) continue;

        token_list = expand_vars(&var_table,token_list);
        token_list = expand_globs(token_list);

        char* cmd_hist = copy_string(line,line_length);
        add_command_to_hist(&history,cmd_hist);

        CmdList cmd_list = parse_commands(token_list);
        free_da_str(&token_list);

        for(int i=0;i<cmd_list.pipe_count;i++){
            CmdPipeline pipeline = cmd_list.pipelines[i];
            char* cont = pipeline.continuation;

            int exit_code = execute_pipeline(pipeline,history,&var_table);
            
            if(cont == NULL) break;
            if(exit_code == 0 && equal_strings(cont,"||")) break;
            if(exit_code == 1 && equal_strings(cont,"&&")) break;
        }

        print_prompt();
    }

    return 0;
}