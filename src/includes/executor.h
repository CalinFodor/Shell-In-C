#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "stringlib.h"
#include "history.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parser.h"
#include "var.h"

#define BASE_FD 10

int echo(StringList tokens);
int pwd();
int cd(StringList tokens);
int history(History history);

int execute_pipeline(CmdPipeline pipeline,History history,VariableTable* var_table);

#endif
