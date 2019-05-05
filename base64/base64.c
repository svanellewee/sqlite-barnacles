#include "sqlite3ext.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static const unsigned char *lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                                     "abcdefghijklmnopqrstuvwxyz"\
                                     "0123456789+/";

#ifndef CALLOC
#define CALLOC(X, Y) (calloc(X, Y))
#endif 

/*
........X..........Y...,,,....Z....
........8..........8..........8....
...6....2.....4....4.....2....6....
...A.......B..........C.......D....
*/
#define SECTIONX(A,B) ((((A) & 0b00111111) << 2) | (((B) & 0b00110000) >> 4))
#define SECTIONY(B,C) ((((B) & 0b00001111) << 4) | (((C) & 0b00111100) >> 2))
#define SECTIONZ(C,D) ((((C) & 0b00000011) << 6) | (((D) & 0b00111111)))
#define DECODED_OUTPUT_LENGTH(X)  ((((((X) - 1) / 4 ) * 3) + 3));
static inline int index_char(unsigned char c) {
  unsigned char *char_ptr = strchr(lookup, c);
  if (char_ptr) {
    return  char_ptr - lookup;
  } else {
      return 0;
  }
}

size_t decode(unsigned const char *input_string, size_t input_string_length, unsigned const char **output_string){
    size_t output_string_length = DECODED_OUTPUT_LENGTH(input_string_length); 
    //char *output = (unsigned char*)MALLOC(sizeof(unsigned char) * output_string_length); 
    char *output = (unsigned char*)calloc(sizeof(unsigned char), output_string_length); 
    memset(output, '\0', output_string_length * sizeof(unsigned char));
    for(int input_count = 0, output_count = 0; input_count < input_string_length; input_count += 4, output_count += 3){
        output[output_count]   = SECTIONX(index_char(input_string[input_count]),  index_char(input_string[input_count+1]));
        output[output_count+1] = SECTIONY(index_char(input_string[input_count+1]),index_char(input_string[input_count+2]));
        output[output_count+2] = SECTIONZ(index_char(input_string[input_count+2]),index_char(input_string[input_count+3]));
    }
    *output_string = output;
    return output_string_length;
}


/*
........X..........Y...,,,....Z....
........8..........8..........8....
...6....2.....4....4.....2....6....
...A.......B..........C.......D....
*/
#define SECTIONA(X)    (((X) & 0b11111100) >> 2)
#define SECTIONB(X,Y) ((((X) & 0b00000011) << 4) | (((Y) & 0b11110000) >> 4))
#define SECTIONC(Y,Z) ((((Y) & 0b00001111) << 2) | (((Z) & 0b11000000) >> 6))
#define SECTIOND(Z)     ((Z) & 0b00111111)
#define ENCODED_OUTPUT_LENGTH(X)  ((((((X) - 1) / 3 ) * 4) + 4));
size_t encode(unsigned const char *input_string, size_t input_string_length, unsigned const char **output_string) {
        int input_count = 0, output_count=0;
        size_t output_string_length = ENCODED_OUTPUT_LENGTH(input_string_length);
        //unsigned char *output = (unsigned char*)MALLOC(sizeof(unsigned char) * output_string_length); 
        unsigned char *output = (unsigned char*)calloc(sizeof(unsigned char), output_string_length); 
        memset(output, '\0', output_string_length * sizeof(unsigned char));
        unsigned const char *cur_pos = input_string;
	unsigned char next_string = '\0';
	unsigned char next_next_string = '\0';
        for(output_count=0, input_count =0; output_count < output_string_length; output_count+=4, input_count+=3) {
                output[output_count] = lookup[SECTIONA(input_string[input_count])];
                next_string = '\0';
                next_next_string = '\0';
                if ((input_string[input_count+1])) {
                    next_string = input_string[input_count+1];
                    if (input_string[input_count+2]) {
                        next_next_string = input_string[input_count+2];
                    }
                }
                output[output_count+1] = lookup[SECTIONB(input_string[input_count], next_string)];
                if (next_string) {
                    output[output_count+2] = lookup[SECTIONC(next_string, next_next_string)];
                } else {
                    output[output_count+2] = '=';
                }
                if (next_next_string) {
                    output[output_count+3] = lookup[SECTIOND(next_next_string)];
                } else {
                    output[output_count+3] = '=';
                }
        }
        *output_string = output;
        return output_string_length;
}

