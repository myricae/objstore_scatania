#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "htable.h"
//#include "msg.h"

extern volatile sig_atomic_t connessi,os_running;
extern pthread_cond_t zero_conn;
extern pthread_mutex_t mtx;
extern node** ht;
void* worker(void * arg);