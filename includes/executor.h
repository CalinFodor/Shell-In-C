#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "stringlib.h"
#include "history.h"
#include <unistd.h>
#include <sys/wait.h>

int echo(StringList tokens);
int pwd();
int cd(StringList tokens);
int history(History history);

//int run_pipeline(StringList tokens);
int run_commands(StringList tokens,History history);

#endif
