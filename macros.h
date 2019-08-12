#ifndef MACROS_H
#define MACROS_H
#define VERBOSE 1
#define SOCKNAME "sock"
#define MAX_COMMAND 255
//#define PATH_MAX 255
#define MAX_HEADER 40
#define MAX_BUFFSIZE 1024 
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