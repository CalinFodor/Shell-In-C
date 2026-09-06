#ifndef HISTORY_H
#define HISTORY_H
#include "stringlib.h"

#define HIST_SIZE 64

typedef DA_StringList History;

History init_history();
char* expand_history(History* history,char line[]);
void add_command_to_hist(History* history,char* command);
void print_hist(History history);

#endif