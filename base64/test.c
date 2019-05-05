#include "base64.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct test_case {
	const char* encoded;
	const char* decoded;
};

typedef struct test_case test_cases[];

test_cases tests = {
	{
		.encoded="TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"\
			  "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"\
			  "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"\
			  "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"\
			  "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=",
		.decoded="Man is distinguished, not only by his reason, but by this singular passion from other animals, "\
                          "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "\
                          "generation of knowledge, exceeds the short vehemence of any carnal pleasure."

	},
	{
		.encoded="dGhlIHJhaW4gaW4gc3BhaW4=",
		.decoded="the rain in spain"
	},
	{
		.encoded="aGVsbG8gd29ybGQ=",
		.decoded="hello world"
	},
	{
		.encoded="aGVsbG8gd29ybA==",
		.decoded="hello worl"
	},
	{ 
		.encoded="MQ==",
		.decoded="1"
	},
	{
		.encoded="MTI=",
		.decoded="12"
	},
	{
		.encoded="MTIz",
		.decoded="123"
	},
	{
		.encoded="MTIzNA==",
		.decoded="1234"
	},
	{
		.encoded="MTIzNDU=",
		.decoded="12345"
	},
	{
		.encoded="MTIzNDU2",
		.decoded="123456"
	},
	{
		.encoded="MTIzNDU2Nw==",
		.decoded="1234567"
	},
	{
		.encoded="MTIzNDU2Nzg=",
		.decoded="12345678"
	},
	{
		.encoded="MTIzNDU2Nzg5",
		.decoded="123456789"
	},
	{
		.encoded="MTIzNDU2Nzg5QQ==",
		.decoded="123456789A"
	},
	{
		.encoded="MTIzNDU2Nzg5QUI=",
		.decoded="123456789AB"
	},
	{
		.encoded=NULL,
		.decoded=NULL
	}
};

int main(int argv, char** argc) {
        struct test_case *test = tests; 
        unsigned const char *decode_result = NULL;
        unsigned const char *encode_result = NULL;
        while(test->encoded != NULL){
                decode(test->encoded, strlen(test->encoded), &encode_result);
                printf("decode() -> %s\n", test->encoded);
		if((strlen(test->decoded) == strlen(encode_result)) &&
				(strncmp(test->decoded, encode_result, strlen(test->decoded)) == 0)) {
			printf("\e[32m" "decode passes\n" "\e[0m");
		} else {
			printf("\e[31m" "decode fails: \n%s VS \n[%s] \n" "\e[0m", test->decoded, encode_result);
		}
                free((void*)encode_result);

                encode(test->decoded, strlen(test->decoded), &decode_result);
                printf("encode() -> %s\n", test->decoded);
		if( (strlen(test->encoded) == strlen(decode_result) ) &&
					(strncmp(test->encoded, decode_result, strlen(test->encoded)) == 0)) {
			printf("\e[32m" "encode passes\n" "\e[0m");
		} else {
			printf("\e[31m" "encode fails \n%s VS \n[%s] \n" "\e[0m", test->encoded, decode_result);
			printf("\e[31m" "encode length comparison in(%d) -> %d VS [%d] \n" "\e[0m", strlen(test->decoded), strlen(test->encoded), strlen(decode_result));
		}
                free((void*)decode_result);

                test++;
                printf("\n");
        }
        return 0;
}

