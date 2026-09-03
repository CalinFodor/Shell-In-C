#ifndef HISTORY_H
#define HISTORY_H
#include "stringlib.h"

typedef StringList History;

History init_history();
char* expand_history(History* history,char line[]);

#endif