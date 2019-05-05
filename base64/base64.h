#ifndef __BASE64__
#define __BASE64__

#include <stdlib.h>

extern size_t decode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string);

extern size_t encode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string);

#endif
