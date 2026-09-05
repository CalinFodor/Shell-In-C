#ifndef PARSER_H
#define PARSER_H
#include "stringlib.h"

typedef enum _RedirDir{
    Input,
    Output
}RedirDir;

typedef enum _ModifyFlag{
    Trunc,
    Append
}ModifyFlag;

typedef struct  _RedirInfo{
    RedirDir redir_dir;
    ModifyFlag modify_flag;
    int fd;
    char* target_file;
} RedirInfo;

typedef struct _ParsedCmd{
    StringList args;
    char* continuation;
    RedirInfo redir_info;
}ParsedCmd;

typedef struct _CmdList{
    ParsedCmd cmd_info[128];
    int idx;
}CmdList;



CmdList parse_commands(StringList tokens);

#endif