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
    TokenList args;
    RedirInfo redir_info;
}ParsedCmd;

typedef struct _CmdPipeline{
    ParsedCmd parsed_cmd[64];
    int cmd_count;
    char* continuation;
}CmdPipeline;

typedef struct _CmdList{
    CmdPipeline pipelines[64];
    int pipe_count;
}CmdList;

CmdList parse_commands(TokenList tokens);

#endif