#include "../includes/parser.h"


bool is_separator(char* s){
    return equal_strings(s,"&&") || equal_strings(s,"||") || equal_strings(s,";");
}

CmdList parse_commands(StringList tokens){

    CmdList cmd_list;
    cmd_list.idx = 0;

    for(int i=0;i<tokens.idx;i++){
        char* token = tokens.elements[i];
        if(is_separator(token)){
            cmd_list.parsed_cmds[cmd_list.idx++].continuation = token;
        }else{
            add_string_to_list(&cmd_list.parsed_cmds[cmd_list.idx].cmds,token);
            if(i == tokens.idx - 1){
                cmd_list.parsed_cmds[cmd_list.idx++].continuation = NULL;
            }
        }
    }

    return cmd_list;
}
