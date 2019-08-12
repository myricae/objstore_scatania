
#include "worker.h"
#include "macros.h"
#include "msg.h"
#include "support.h"
//#include "htable.h"
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
client_t *client;
//Clean-up routines
static void dec_signal(void* arg){
    if(VERBOSE) fprintf(stderr,"pop dec_signal\n");
    connessi--;
    if (connessi==0) pthread_cond_signal(&zero_conn);
}
static void cclose(void* arg){
    if(VERBOSE) fprintf(stderr,"pop cclose\n");
    int cfd=*((int *)arg);
    close(cfd);
}
static void ffree(void* arg){
    if(VERBOSE) fprintf(stderr,"pop ffree\n");
    if(arg!=NULL) free(arg);
}
static void freemsg(void* arg){
    if(VERBOSE) fprintf(stderr,"pop freemsg\n");
    msg *m=(msg*)arg;
    if(m!=NULL){
        free(m->data);
        free(m->name);
        free(m);
    }
}

//Worker code
void* worker(void * arg){
    client=(client_t*)malloc(sizeof(client_t));
    pthread_cleanup_push(&ffree,client);
    if(VERBOSE) fprintf(stderr,"push free(client_t)\n");

    client->on=1;
    client->reg=0;
    client->fd=*((int *)arg);
    pthread_cleanup_push(&cclose,(void*)&client->fd);
    if(VERBOSE) fprintf(stderr,"push cclose(fd)\n");
    
    connessi++;
    pthread_cleanup_push(&dec_signal,(void*)NULL);
    if(VERBOSE) fprintf(stderr,"push dec signal()\n");

    
    char* request=(char*)malloc(sizeof(char)*MAX_COMMAND);
    pthread_cleanup_push(&ffree,(void*)request);
    if(VERBOSE) fprintf(stderr,"push free(request)\n");

    
    client->path=malloc(sizeof(char)*NAME_MAX); //TO DO realloc(client->path) after client registration
    pthread_cleanup_push(&ffree,(void*)client->path);
    if(VERBOSE) fprintf(stderr,"push free(client_t->path)\n");

    msg* message=NULL;

    while (os_running && client->on) {
        if(s_readline(client->fd,request,MAX_COMMAND)) {
            if(VERBOSE) printf("worker: while->request: %s\n",request);
            message=tomsg(request,client->fd);
            pthread_cleanup_push(&freemsg,(void*)message);
            if(message!=NULL && message->valid){
                if(VERBOSE) printmsg(message);
                if(message->com==REGISTER){
                    if(!client->reg){
                        if(!exists(message->name,ht)){
                            add(message->name,ht);
                            client->name=malloc(sizeof(message->name));
                            strcpy(client->name,message->name);

                            client->path=malloc(strlen(client->name)+strlen(DATADIR)+1+1);//+1 for final "/",+1 for "\0" terminator
                            strcpy(client->path,DATADIR);
                            strcat(client->path,client->name);
                            strcat(client->path,"/");

                            if(mkdir(client->path,0777)==-1) {
                                sendko(client->fd,"Creating user's directory.");
                            }
                        }
                        client->reg=1;
                    }
                    else{
                        sendko(client->fd,"Client already logged in.");
                    }
                    sendok(client->fd);
                }
                else{
                    if(!client->reg){ //not registered
                        sendko(client->fd,"Client has to send registration request first.");
                    } 
                    else{
                        if(message->com==LEAVE){
                            client->reg=0;
                            client->on=0;
                            sendok(client->fd);
                        }
                        else if(message->com==STORE){
                            FILE* new;//new file pointer on which that data block is written
                            char* full_path=malloc(strlen(client->path)+strlen(message->name)+1);
                            strcpy(full_path,client->path);
                            strcat(full_path,message->name);
                            
                            if((new=fopen(full_path,"wb"))==NULL) sendko(client->fd,"Problems creating file.");
                            else{
                                int wrote;
                                if((wrote=fwrite(message->data,1,message->len,new))!=message->len) sendko(client->fd,"Problems writing data to file.");
                                else sendok(client->fd);
                            }
                            free(full_path);
                            fclose(new);
                        }
                        else if(message->com==RETRIEVE){
                            char* full_path,*data;
                            FILE* f;
                            int len;

                            //preparing file relative path+name
                            full_path=malloc(strlen(client->path)+strlen(message->name)+1);
                            strcpy(full_path,client->path);
                            strcat(full_path,message->name);

                            if((f=fopen(full_path,"rb"))==NULL) sendko(client->fd,"Problems opening file requested by retrieve.");
                            else{
                                if((len=fseek(f,0,SEEK_END))==-1) sendko(client->fd,"Zero-dimensioned file.");
                                else{
                                    rewind(f);
                                    data=malloc(len);
                                    if(fread(data,1,len,f)!=len) sendko(client->fd,"Problems reading file requested by retrieve.");
                                    else sendok_retr(client->fd,len,data);
                                }
                            }
                            free(full_path);
                        }
                        else if(message->com==DELETE){
                            char* full_path=malloc(strlen(client->path)+strlen(message->name)+2);
                            strcpy(full_path,client->path);
                            strcat(full_path,message->name);
                            strcat(full_path,"/");
                            
                            int temp;
                            if((temp=unlink(full_path))==-1){
                                if(errno==ENOENT){
                                    sendko(client->fd,"File does not exist.");
                                }
                                else{
                                    sendko(client->fd,"Problems deliting file");
                                }
                            }
                            else sendok(client->fd);
                            free(full_path);
                        }
                        else { //unknown option
                            sendko(client->fd,"Unknown protocol.");
                        }
                    }
                }
            }
            else sendko(client->fd,"Invalid message.");
            pthread_cleanup_pop(1);
        }
        else{
                os_running=0;
                if(VERBOSE) fprintf(stderr,"worker: s_readline fallita, running=0.\n");
        }
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_detach(pthread_self());
    if(VERBOSE) printf("thread %d exiting: now %d connected\n",(int)pthread_self(),connessi);
    pthread_exit((void*)NULL);
}