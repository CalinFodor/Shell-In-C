#ifndef PARSER_H
#define PARSER_H
#include "stringlib.h"

typedef struct _ParsedCmd{
    StringList cmds;
    char* continuation;
}ParsedCmd;

typedef struct _CmdList{
    ParsedCmd parsed_cmds[128];
    int idx;
}CmdList;

CmdList parse_commands(StringList tokens);

#endif