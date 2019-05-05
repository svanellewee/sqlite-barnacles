#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base64.h"

// Poor man's dependency injection..
#define CALLOC(X,Y) (sqlite3_malloc(X*Y))

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
