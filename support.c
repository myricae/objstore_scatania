#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include "support.h"
#include "macros.h"

client_t *client;
void sendko(int fd,char* msg){
    char* buf=malloc(strlen(msg)+5+1);
    sprintf(buf,"KO %s\n",msg);
    
    int wrote,len=strlen(buf);
    if((wrote=send(fd,buf,len,0))!=len) {
        fprintf(stderr,"sendko: send to fd %d failed, wrote %d bytes\n",fd,wrote);
        client->on=0;
    }
    free(buf);
}
void sendok(int fd){
    int wrote;
    if((wrote=send(fd,"OK \n",5,0))!=5) {
        fprintf(stderr,"sendok: send to fd %d failed, wrote %d bytes\n",fd,wrote);
        client->on=0;
    }
}
void sendok_retr(int fd,int len,char* data){
    char* length=malloc(sizeof(char)*32);
    sprintf(length,"%d",len);
    int digits=strlen(length);
    free(length);

    int header=strlen("DATA ")+digits+strlen(" \n ");
    char* buf=malloc(sizeof(char)*(header+len+1));
    sprintf(buf,"DATA %d \n ",len); 
    strncat(buf+header,data,len);
    int size=header+len;
    if(send(fd,buf,size,0)!=size) {
        if(VERBOSE) fprintf(stderr,"sendok_retr: send failed.\n");
        sendko(fd,"Problems sending retrieve response header.");
    }
    free(buf);
}
int s_readline(int fd, char *buf,int capacity){
    int res;
    
    struct pollfd fds[1];
    fds[0]=(struct pollfd){fd, POLLIN, 0};
    int len=0,time=0;

    while((res=poll(fds,1,10))==0) {
        time++;
        if(VERBOSE && !time%20) fprintf(stderr,"s_readline: poll on socket timed out.\n");
        if(time>5000) {//timeout fixed at 5s
            fprintf(stderr,"s_readline: server not responding.\n");
            goto end; 
        }
    }  
    if(res==-1) perror("s_readline, poll errno: ");
    else {//event occurred on socket
        if(POLLIN & fds[0].revents){//there is data to read on socket
            len=recv(fd,buf,capacity,0);
            if(VERBOSE){
                if(len==0) fprintf(stderr,"s_readline: red 0 bytes.\n");
                else if(len<0) fprintf(stderr,"s_readline: client disconnected.\n");
                else fprintf(stderr,"s_readline: red %d bytes.\n",len);
            }
        }
        else fprintf(stderr,"s_readline: other event on socket occured\n");//should not happen? Didn't request other events
    }
    end:return len;
}