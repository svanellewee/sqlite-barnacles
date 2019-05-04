#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base64.h"
static const unsigned char *lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                                     "abcdefghijklmnopqrstuvwxyz"\
                                     "0123456789+/";


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

size_t decode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string){
    size_t output_string_length = DECODED_OUTPUT_LENGTH(input_string_length); 
    char *output = (unsigned char*)sqlite3_malloc(sizeof(unsigned char) * output_string_length); 
    memset(output, '\0', output_string_length);
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
size_t encode(unsigned const char *input_string, size_t input_string_length, unsigned char **output_string) {
        int input_count = 0, output_count=0;
        size_t output_string_length = ENCODED_OUTPUT_LENGTH(input_string_length);
        unsigned char *output = (unsigned char*)sqlite3_malloc(sizeof(unsigned char) * output_string_length); 
        memset(output, '\0', output_string_length * sizeof(unsigned char));
        unsigned const char *cur_pos = input_string;
        for(output_count=0, input_count =0; output_count < output_string_length; output_count+=4, input_count+=3) {
                output[output_count] = lookup[SECTIONA(input_string[input_count])];
                unsigned char next_string = '\0';
                unsigned char next_next_string = '\0';
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


static void base64_encode(
      sqlite3_context *context,
      int argc,
      sqlite3_value **argv
    ){
      unsigned char *output_string = NULL;
      const unsigned char *input_string = NULL;
      size_t input_string_length = 0, output_string_length = 0;

      if( sqlite3_value_type(argv[0]) == SQLITE_NULL ) return;
      input_string = (const unsigned char*)sqlite3_value_text(argv[0]);
      input_string_length = sqlite3_value_bytes(argv[0]);
      output_string_length = encode(input_string, input_string_length, &output_string);
      if( output_string == NULL ){
          sqlite3_result_error_nomem(context);
          return;
      }
      sqlite3_result_text(context, output_string, output_string_length, SQLITE_TRANSIENT);
      sqlite3_free(output_string);
}

static void base64_decode(
      sqlite3_context *context,
      int argc,
      sqlite3_value **argv
    ){
      unsigned char *output_string = NULL;
      const unsigned char *input_string = NULL;
      size_t input_string_length = 0, output_string_length = 0;

      if( sqlite3_value_type(argv[0]) == SQLITE_NULL ) return;
      input_string = (const unsigned char*)sqlite3_value_text(argv[0]);
      input_string_length = sqlite3_value_bytes(argv[0]);
      output_string_length = decode(input_string, input_string_length, &output_string);
      if( output_string == NULL ){
          sqlite3_result_error_nomem(context);
          return;
      }
      sqlite3_result_text(context, output_string, output_string_length, SQLITE_TRANSIENT);
      sqlite3_free(output_string);
}

#ifdef _WIN32
    __declspec(dllexport)
#endif
int sqlite3_base_init(
      sqlite3 *db,
      char **pzErrMsg,
      const sqlite3_api_routines *pApi
){
      int rc = SQLITE_OK;
      SQLITE_EXTENSION_INIT2(pApi);
      (void)pzErrMsg;  /* Unused parameter */
      rc = sqlite3_create_function(db, "base64_encode", 1, SQLITE_UTF8, 0, base64_encode, 0, 0);
      rc = sqlite3_create_function(db, "base64_decode", 1, SQLITE_UTF8, 0, base64_decode, 0, 0);
      return rc;
}
