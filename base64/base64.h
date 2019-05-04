#ifndef __BASE64__
#define __BASE64__

#include "sqlite3ext.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t decode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string);

size_t encode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string);
#endif
