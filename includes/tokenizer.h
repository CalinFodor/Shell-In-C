#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "stdio.h"
#include "stringlib.h"

typedef enum _TokenizeState{
    DEFAULT,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} TokenizeState;

TokenList tokenize(char line[]);

#endif