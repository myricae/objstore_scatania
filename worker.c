
#include <stdlib.h>
#include "worker.h"
#include "macros.h"
#include "msg.h"
#include "support.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
client_t *client;
#define shutornot(x) if((x)==0) client->reg=client->on=0
char* buf=NULL;

int handle_store(FILE* out,int fd,int datalen){
    buf=malloc(sizeof(char)*MAX_BUFFSIZE);
    memset(buf,0,MAX_BUFFSIZE);
    int red,wrote;
    int firstwrite,more=1;
    //don't add \0 at the end of file. dimension has to remain the same
    if(datalen<MAX_BUFFSIZE){
        more=0;
        firstwrite=datalen;
    }
    if((red=readn(fd,buf,firstwrite))>0){
        if(red!=firstwrite){
            fprintf(stderr,"handle_store: first readn red %d/%d bytes.\n",red,firstwrite);
            free(buf);
            return 0;
        }
        if((wrote=fwrite(buf,1,red,out))<=0){
            fprintf(stderr,"handle_store: first fwrite wrote %d/%d bytes",red,firstwrite);
            perror(": ");
            free(buf);
            return 0;
        }
        if(!more) return 1;
    }
    else{
        fprintf(stderr,"handle_store: first readn failed. Red %d bytes.\n",red);
        free(buf);
        return 0;
    }
    //we have more data to read from socket
    int left=datalen-wrote,size;
    while(left>0){
        size=left>MAX_BUFFSIZE?MAX_BUFFSIZE:left;
        if((red=readn(fd,buf,size))>0){
            left-=red;
            if((wrote=fwrite(buf,1,red,out))<=0){
                fprintf(stderr,"handle_store: fwrite wrote %d bytes off %d total ",red,size);
                perror(": ");
                free(buf);
                return 0;
            }
        }
        else{
            fprintf(stderr,"handle_store: readn failed. Last read: %d bytes. Total red: %d/%d\n",red,datalen-left,datalen);
            free(buf);
            return 0;
        }
    }
    free(buf);
    return 1;
}

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
        if(m->name!=NULL) free(m->name);
        free(m);
    }
}
static void client_cleanup(void* arg){
    if(VERBOSE) fprintf(stderr,"pop client_cleanup\n");
    client_t *c=(client_t*)arg;
    if(c->name)free(c->name);
    if(c->path)free(c->path);
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
    
    char* request=(char*)malloc(sizeof(char)*MAX_HEADER);
    pthread_cleanup_push(&ffree,(void*)request);
    if(VERBOSE) fprintf(stderr,"push ffree(request)\n");

    pthread_cleanup_push(&ffree,(void*)buf);
    if(VERBOSE) fprintf(stderr,"push ffree(buf)\n");

    msg* message=NULL;
    int headerlen;
    while (os_running && client->on) {
        if((headerlen=getheader(client->fd,request,MAX_HEADER))>0) {

            message=tomsg(request,client->fd);
            pthread_cleanup_push(&freemsg,(void*)message);

            if(message!=NULL && message->valid){
                if(VERBOSE) printmsg(message);
                if(message->com==REGISTER){
                    int res=1;
                    if(!client->reg){
                        client->name=malloc(strlen(message->name)+1);
                        strcpy(client->name,message->name);
                        client->path=(char*)malloc(strlen(client->name)+strlen(DATADIR)+1+1);//+1 for final "/",+1 for "\0" terminator
                        sprintf(client->path,"%s%s/",DATADIR,client->name);
                        if(VERBOSE) {
                            if(client->name!=NULL)fprintf(stderr,"client->name: %s\n",client->name);
                            else fprintf(stderr,"client->name=NULL\n");
                            if(client->path!=NULL) printf("client->path: %s\n",client->path);
                            else fprintf(stderr,"client->path=NULL\n");
                        }
                        if(!exists(client->name,ht)){
                            if((res=add(client->name,ht))==-1){
                                if(VERBOSE) fprintf(stderr,"worker.c: add failed.\n");
                            }
                            else{
                                if((res=mkdir(client->path,0777))==-1){
                                    perror("worker: register: ");
                                    shutornot(sendko(client->fd,"Creating user's directory."));
                                }
                            }
                        }
                        if(res>=0){
                            client->reg=1;
                            shutornot(sendok(client->fd));
                        }
                    }
                    else{
                        if(VERBOSE){
                            if(client->name!=NULL) fprintf(stderr,"client %s already logged in.\n",client->name);
                            else fprintf(stderr,"!client->reg: client->name=NULL\n");
                        }
                        if(client->fd) {shutornot(sendok(client->fd));}
                    }
                }
                else{
                    if(client->reg==0){ //not registered
                        shutornot(sendko(client->fd,"Client has to send registration request first."));
                    } 
                    else{
                        if(message->com==LEAVE){
                            client->reg=client->on=0;
                            shutornot(sendok(client->fd));
                        }
                        else if(message->com==STORE){
                            char* full_path=malloc(strlen(client->path)+strlen(message->name)+1);
                            full_path[0]='\0';
                            sprintf(full_path,"%s%s",client->path,message->name);
                            FILE* out;
                            
                            if((out=fopen(full_path,"ab"))==NULL) {shutornot(sendko(client->fd,"Problems creating file."));}
                            else{
                                int res;
                                if((res=handle_store(out,client->fd,message->len))==0){
                                    if(VERBOSE) fprintf(stderr,"handle_store returned %d\n",res);
                                    shutornot(sendko(client->fd,"Store failed."));
                                }
                                else {shutornot(sendok(client->fd));}
                                fclose(out);
                            }
                            free(full_path);
                        }
                        else if(message->com==RETRIEVE){
                            char* full_path;
                            FILE* f;
                            int datalen;
                            if(VERBOSE){
                                if(client->path==NULL) fprintf(stderr,"worker: retrieve: client->path==NULL\n");
                                if(message->name==NULL) fprintf(stderr,"worker: retrieve: message->name==NULL\n");
                            }
                            //preparing file relative path+name
                            full_path=malloc(strlen(client->path)+strlen(message->name)+1);
                            sprintf(full_path,"%s%s",client->path,message->name);
                            if(VERBOSE) fprintf(stderr,"full_path=%s\n",full_path);
                            if((f=fopen(full_path,"rb"))==NULL) {shutornot(sendko(client->fd,"Problems opening requested file."));}
                            else{
                                fseek(f,0,SEEK_END);
                                if((datalen=ftell(f))==-1) {shutornot(sendko(client->fd,"Zero-dimensioned file."));}
                                else{
                                    rewind(f);
                                    shutornot(sendok_retr(client->fd,datalen,f));
                                }
                                fclose(f);
                            }
                            free(full_path);
                        }
                        else if(message->com==DELETE){
                            char* full_path=malloc(strlen(client->path)+strlen(message->name)+2);
                            sprintf(full_path,"%s%s",client->path,message->name);
                            
                            if(VERBOSE) fprintf(stderr,"%s\n",full_path);
                            int temp;
                            if((temp=unlink(full_path))==-1){
                                if(errno==ENOENT){
                                    shutornot(sendko(client->fd,"File does not exist."));
                                }
                                else{
                                    if(VERBOSE) perror("os_server/delete: ");
                                    shutornot(sendko(client->fd,"Problems deliting file."));
                                }
                            }
                            else {shutornot(sendok(client->fd));}
                            free(full_path);
                        }
                        else { //unknown option
                            shutornot(sendko(client->fd,"Unknown option."));
                        }
                    }
                }
            }
            else {
                printf("Invalid message\n");
                shutornot(sendko(client->fd,"Invalid message."));
            }
            pthread_cleanup_pop(1);
        }
        else{
                client->on=0;
                if(VERBOSE) fprintf(stderr,"worker: getheader fallita, running=0.\n");
        }
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    pthread_detach(pthread_self());
    if(VERBOSE) printf("thread %ld exiting: now %d connected\n",pthread_self(),connessi);
    pthread_exit((void*)NULL);
}