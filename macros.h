#ifndef MACROS_H
#define MACROS_H
#define VERBOSE 1
#define TIMEOUT 200
#define SOCKNAME "sock"
#define MAX_HEADER 512
#define MAX_BUFFSIZE 2048
#define OKO_MAXLEN 64
#define DATADIR "./data/"

#define ifmeno_usr(c,s) \
    if((c) == -1) {\
        fprintf(stderr,s);\
        exit(EXIT_FAILURE);\
    }
#define ifminus(c,s) \
    if((c) == -1) {\
        fprintf(stderr,s);\
    }

#endif