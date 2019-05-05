CC := /usr/bin/docker run --rm -v "${PWD}":/usr/src/myapp -w /usr/src/myapp gcc:8.3 gcc
DL_SUFFIX := 
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	DL_SUFFIX = so
endif
ifeq ($(UNAME_S),Darwin)
	DL_SUFFIX = dylib
endif

base64.${DL_SUFFIX}: base64.c wrapper.c
	${CC} -fPIC  -g -shared *.c -o $@

clean: 
	rm -fr *.so *.o
