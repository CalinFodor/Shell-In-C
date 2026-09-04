#ifndef GLOB_H
#define GLOB_H
#include "stringlib.h"
#include "fs.h"
#include <unistd.h>

StringList expand_globs(StringList tokens);

#endif