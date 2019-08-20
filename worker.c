
#include <stdlib.h>
#include "worker.h"
#include "macros.h"
#include "msg.h"
#include "support.h"
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
client_t *client;
//Clean-up routines
static void dec_signal(void* arg){
    if(VERBOSE) fprintf(stderr,"pop dec_signal\n");
    connessi--;
    if (connessi==0) pthread_cond_signal(&zero_conn);
}
static void ffree(void* arg){
    if(VERBOSE) fprintf(stderr,"pop ffree\n");
    if(arg!=NULL) free(arg);
}
static void freemsg(void* arg){
    if(VERBOSE) fprintf(stderr,"pop freemsg\n");
    msg *m=(msg*)arg;
    if(m!=NULL){
        if(m->data!=NULL) free(m->data);
        if(m->name!=NULL) free(m->name);
        free(m);
    }
}
static void client_cleanup(void* arg){
    if(VERBOSE) fprintf(stderr,"pop client_cleanup\n");
    client_t *c=(client_t*)arg;
    free(c->name);
    free(c->path);
    close(client->fd);
    free(c);
}

//Worker code
void* worker(void * arg){
    connessi++;
    pthread_cleanup_push(&dec_signal,(void*)NULL);
    if(VERBOSE) fprintf(stderr,"push dec signal()\n");
 
    client=(client_t*)malloc(sizeof(client_t));
    client->on=1;
    client->reg=0;
    client->name=client->path=NULL;
    client->fd=*((int *)arg);
    pthread_cleanup_push(&client_cleanup,(void*)client);
    if(VERBOSE) fprintf(stderr,"push client_cleanup(client_t)\n");
    
    char* request=(char*)malloc(sizeof(char)*MAX_COMMAND);
    pthread_cleanup_push(&ffree,(void*)request);
    if(VERBOSE) fprintf(stderr,"push ffree(request)\n");

    msg* message=NULL;

    while (os_running && client->on) {
        int red;
        if((red=s_readline(client->fd,request,MAX_COMMAND))) {
            request[red]='\0';
            if(VERBOSE) printf("worker: while->request: %s\n",request);

            message=tomsg(request,client->fd);
            pthread_cleanup_push(&freemsg,(void*)message);

            if(message!=NULL && message->valid){
                if(VERBOSE) printmsg(message);
                if(message->com==REGISTER){
                    int res;
                    if(!client->reg){
                        //TO DO: MUTEX SU ACCESSI A HT
                        if(!exists(message->name,ht)){
                            add(message->name,ht);
                            client->name=malloc(strlen(message->name)+1);
                            strcpy(client->name,message->name);
                            if(VERBOSE) printf("client>name: %s\n",message->name);
                            if(VERBOSE) printf("&(message->name)=%p\n",(void*)message->name);
                            client->path=(char*)malloc(strlen(client->name)+strlen(DATADIR)+1+1);//+1 for final "/",+1 for "\0" terminator
                            if(VERBOSE) printf("&(client->path)=%p\n",(void*)client->path);
                            strcpy(client->path,DATADIR);
                            strcat(client->path,client->name);
                            strcat(client->path,"/");
                            
                            if((res=mkdir(client->path,0777))==-1) {
                                sendko(client->fd,"Creating user's directory.");
                            }
                        }
                        if(res==-1) client->reg=0;
                        else client->reg=1;
                    }
                    else{
                        sendko(client->fd,"Client already logged in.");
                    }
                    sendok(client->fd);
                }
                else{
                    if(client->reg==0){ //not registered
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
                                if(fwrite(message->data,1,message->len,new)!=message->len) sendko(client->fd,"Problems writing data to file.");
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
                            
                            if(VERBOSE) fprintf(stderr,"%s\n",full_path);
                            int temp;
                            if((temp=unlink(full_path))==-1){
                                if(errno==ENOENT){
                                    sendko(client->fd,"File does not exist.");
                                }
                                else{
                                    if(VERBOSE) perror("os_server/delete: ");
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
                client->on=0;
                if(VERBOSE) fprintf(stderr,"worker: s_readline fallita, running=0.\n");
        }
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    pthread_detach(pthread_self());
    if(VERBOSE) printf("thread %ld exiting: now %d connected\n",pthread_self(),connessi);
    pthread_exit((void*)NULL);
}