#ifndef GLOB_H
#define GLOB_H
#include "stringlib.h"
#include "fs.h"
#include <unistd.h>

TokenList expand_globs(TokenList tokens);

#endif