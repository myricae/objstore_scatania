#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <limits.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <ftw.h>
#include "support.h"
#include "macros.h"
#include "worker.h"
#include "htable.h"

//cleanup socketaddress
//aggiungere qualche fflush quà e là
//risolvere memory leak ad uscita imprevista su serve


//OTTIMIZZARE: fread/fwrite ritorna 0, passare anche il parametro con cui ci si aspetta errore?


//ritardare la chiusura del server fino a che il numero di client connessi==0, NO ATTESA ATTIVA -> Usare cond var
//connessi++ quando viene eseguita una register
volatile sig_atomic_t connessi=0,os_running=1;
int size=0,obj=0; 
node** ht=NULL; //mutex sugli accessi a questa qui
//Il salvataggio della HT deve avvenire quando il server termina a causa di un segnale
pthread_cond_t zero_conn=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
sigset_t set;

int fn (const char *fpath, const struct stat *sb,int typeflag){
     if(typeflag==FTW_F) {
         obj++;
         size+=(int)sb->st_size;
     }
     return 1;
 }

static void* stats(void* arg){
    pthread_detach(pthread_self());
    ftw(DATADIR,&fn,500);
    printf("Connected users: %d, OS total size: %d, OS total objects: %d",connessi,size,obj);
    size=obj=0;
    pthread_exit((void*)NULL);
}
//signal listener
static void* sighandler(void* arg){
    int sig;
    pthread_t tid;
    start: if(sigwait(&set,&sig)>0){fprintf(stderr,"Invalid signal mask.\n");}
    else{
        if(VERBOSE) printf("\nAccolto segnale: %d\n",sig);
        switch(sig){
            case SIGINT||SIGTERM:
                os_running=0;
                if(VERBOSE) printf("os_running=0\n");
                break;
            case SIGUSR1:
                pthread_create(&tid,NULL,stats,NULL);
                break;
            default:      
                break;  
        }
        if(sig==SIGUSR1) goto start;
    }
    if(VERBOSE) printf("Sighandler uscito.\n");
    pthread_exit((void*)NULL);
}
static void* dispatcher(void* arg){
    pthread_detach(pthread_self());
    int sfd=*(int*)arg,fdc;
    pthread_t tid;
    while(os_running){
        if((fdc=accept(sfd,NULL,0))==-1) {if(os_running) perror("main: Accept failed. \n"); break;}//recheck on os_running in case value has
        else pthread_create(&tid,NULL,worker,(void*)&fdc);                                 //changed inside "while" while "accept" was blocking
    }
    if(VERBOSE) printf("Dispatcher uscito.\n");
    pthread_exit((void*)NULL);
}
int main() {
    sigfillset(&set);
    //DEBUG
    sigdelset(&set,SIGABRT);
    pthread_sigmask(SIG_BLOCK,&set,NULL);

    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGTERM);
    sigaddset(&set,SIGUSR1);

    pthread_t disp,handler_t;
    pthread_create(&handler_t,NULL,sighandler,(void*)NULL);

    struct stat unused;
    if(!stat(SOCKNAME,&unused)){
        unlink(SOCKNAME);
    }

    int sfd;
    struct sockaddr_un address;
    memset(&address,0,sizeof(struct sockaddr_un));

    address.sun_family=AF_UNIX;
    strcpy(address.sun_path,SOCKNAME);

    ifminus(sfd=socket(AF_UNIX,SOCK_STREAM,0),"main: Creating socket. \n");
    ifminus(bind(sfd,(struct sockaddr*)&address,sizeof(address)),"main: Bind failed. \n");
    ifminus(listen(sfd,SOMAXCONN),"main: Listen failed.\n");

    
    ht=init(ht);
    
    if(VERBOSE) printf("main: Sto ascoltando\n");
    //client listener
    pthread_create(&disp,NULL,dispatcher,(void*)&sfd);

    if(VERBOSE) printf("main: Prima di joinare sighandler\n");
    pthread_join(handler_t,NULL);
    
    pthread_mutex_lock(&mtx);
    while(connessi>0) pthread_cond_wait(&zero_conn,&mtx);
    pthread_mutex_unlock(&mtx);
    
    if(VERBOSE) printf("main: %d client connessi.\n",(int)connessi);
    destroy(ht);
    close(sfd);
    if(VERBOSE) printf("main: ht distrutta, connessione chiusa.\n");
    if(unlink(SOCKNAME)==-1){
        if(errno==ENOENT){
            fprintf(stderr,"main: Socket not found.\n");
        }
        else{
            fprintf(stderr,"main: Problems deliting socket. \n");
        }
    }
    if(VERBOSE) printf("main: socket file deleted, exiting.\n");
    return 0;
}