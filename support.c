#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include "support.h"
#include "macros.h"

extern client_t *client;
int readn(int fd, char *buf, int n){
    memset(buf,0,n);

    int  left=n,red;
    char* ptr=buf;

    if(waitdata(fd)){
        while(left>0){
            if ((red=read(fd, ptr, left))<0){
                if(errno==EINTR)//interrupt by signal, we could still have data to read
                    red=0;
                else{
                    perror("readn: ");
                    return -1;
                }
            }
            else if(!red) break;
            left-=red;
            ptr+=red;
        }
        return n-left;
    }
    else return -1;
}
int writen(int fd,char* buf,int n){
    int left=n,wrote;
    char* ptr=buf;
    while(left>0){
        if((wrote=write(fd,ptr,left))<=0){
            if(wrote<0){
                if(errno==EINTR) wrote=0;
                else{
                    perror("writen: ");
                    return -1;
                }
            }
            else if (wrote==0){
                fprintf(stderr,"writen: wrote 0 bytes.\n");
                return -1;
            }
        }
        left-=wrote;
        ptr+=wrote;
    }
    return n;
}
int sendko(int fd,char* msg){
    if((dprintf(fd,"KO %s \n",msg))<0) {
        if(VERBOSE)
            perror("sendko,shutting off client: dprintf: ");
        return 0;
    }
    return 1;
}
int sendok(int fd){
    if((dprintf(fd,"OK \n"))<0) {
        if(VERBOSE)
            perror("sendok,shutting off client: dprintf: ");
        return 0;
    }
    return 1;
}
int sendok_retr(int fd,int datalen,FILE* f){
    if(dprintf(fd,"DATA %d \n ",datalen)<0){
        if(VERBOSE) {
            perror("sendok_retr: dprintf: ");
        }
        return 0;
    }
    else{
        char buf[datalen];
        int red;
        if((red=fread(buf,1,datalen,f))!=datalen){
            if(VERBOSE){
                if(red<0) perror("sendok_retr: fread: ");
                else fprintf(stderr,"sendok_retr: fread red %d bytes instead of datalen %d\n",red,datalen);
            }
            return 0;
        }
        else{
            if(writen(fd,buf,datalen)<0){
                fprintf(stderr,"sendok_retr: writen failed.\n");
                return 0;
            }
        }
    }
    return 1;
}
int waitdata(int fd){
    int res;
    
    struct pollfd fds[1];
    fds[0]=(struct pollfd){fd, POLLIN, 0};
    int time=0;

    while((res=poll(fds,1,10))==0) {
        time++;
        if(VERBOSE && !time%20) fprintf(stderr,"waitdata: poll on socket timed out.\n");
        if(time>TIMEOUT) {//timeout fixed at 2s: 10ms*200=2s
            if(VERBOSE)fprintf(stderr,"waitdata: server not responding.\n");
            break;
        }
    }  
    if(res==-1) {
        perror("waitdata: poll: ");
        return 0;
    }
    else {//event occurred on socket
        if(POLLIN & fds[0].revents) return 1;//there is data to read on socket
        if(POLLHUP & fds[0].revents) {
            perror("waitdata: poll: ");
            return 0;
        }
    }
    return 0;
}
int getheader(int fd,char* buf,int maxlen){
    memset(buf,0,maxlen);

    if(!waitdata(fd)){
        fprintf(stderr,"getheader: waitdata failed, no data to read on socket.\n");
        return -1;
    }
    int n,red;
    for(n=1;n<maxlen;n++){//n<maxlen because we terminate the header with a \0
        if((red=read(fd,buf+n-1,1))<0){
            if(errno==EINTR) n--;
            else return -1;
        }
        else if(red==0){//no more data to read
            *(buf+n-1)='\0';
            return n-1;
        }
        else{//1 byte has been red
            if(*(buf+n-1)=='\n') break;
        }
    }
    *(buf+n)='\0';
    if(VERBOSE) fprintf(stderr,"getheader result: -%s-",buf);
    return n;
}