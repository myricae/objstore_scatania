#include "objstore.h"
#include "support.h"

#define ERRSTR_LEN 128
//TO DO: convertire write in send
static int cfd=-1,ret;
static int connected=0,letti;
char* request=NULL,*answer=NULL;
char* errstr[ERRSTR_LEN];

#define except_retrieve(cfd,answer,fun)\
    if((letti=s_readline(cfd,answer,MAX_HEADER))){\
        if(letti){\
            answer[letti]='\0';\
            fprintf(stderr,"%s",answer);\
            if(answer[0]=='O') ret=1;\
            else ret=0;\
        }\
        else ret=0;\
    }\
    else{\
        fprintf(stderr,"%s: impossible to read from server.\n",fun);\
        ret=0;\
    }\
    free(answer);

int os_connect(char *name){
    
    if(!connected){
        struct sockaddr_un addr;      
        strcpy(addr.sun_path, SOCKNAME);
        addr.sun_family=AF_UNIX;
        ifmeno_usr(cfd=socket(addr.sun_family,SOCK_STREAM,0),"os_connect: problems creating socket");
        ifmeno_usr(connect(cfd,(struct sockaddr*)&addr,sizeof(addr)),"os_connect: problems with connect: ");
        connected=1;
        //int len=strlen("REGISTER  \n")+strlen(name);
        //request=calloc(len,sizeof(char));
        answer=malloc(64);//40= maximum server answer size for STORE
        dprintf(cfd,"REGISTER %s \n",name);
        //int scritti;
        //if((scritti=send(cfd,request,len,0))!=len) fprintf(stderr,"os_connect: problems sending request.");
        //if(VERBOSE) printf("Scritti sul socket=strlen(request)? %d\n",scritti==len);
        except_retrieve(cfd,answer,"os_connect");
    }
    else {
        fprintf(stderr,"os_connect: already connected.\n");
        ret=0;
    }
    return ret;
}

int os_disconnect(){
    if(connected){
        //request=malloc(1);//for except_retrieve macro compatibility
        answer=malloc(sizeof(char)*64);
        dprintf(cfd,"LEAVE \n");
        except_retrieve(cfd,answer,"os_disconnect");
        connected=0;

        close(cfd);
        ret=1;
    }
    else{
        fprintf(stderr,"os_disconnect: client needs to call os_connect first.\n");
        ret=0;
    }
    return ret;
}
int os_delete(char *name){
    if(name && connected) {
        //int len=(strlen("DELETE  \n")+strlen(name));
        //request=calloc(len,sizeof(char));
        answer=malloc(sizeof(char)*64);
        //snprintf(request,len,"DELETE %s \n",name);
        if(dprintf(cfd,"DELETE %s \n",name)) {
            fprintf(stderr,"os_delete: write on socket failed.\n");
            free(request);
            free(answer);
            ret=0;
        }
        else{
            except_retrieve(cfd,answer,"os_delete");
        }
    }
    else {
        if(!connected) printf("os_delete: not connected.\n");
        if(!name) printf("os_delete: invalid name.\n");
        ret=0;
    }
    return ret;
}
int os_store(char *name, void *block, size_t len){
    if(name && connected) {
        answer=malloc(sizeof(char)*64);
        int dim;
        char* request=malloc(sizeof(char)*(1+MAX_HEADER +len));

        sprintf(request,"STORE %s %zu \n %s",name,len,(char*)block);
        dim=strlen(request);
        if(send(cfd,request,dim,0)!=dim){
            fprintf(stderr,"os_store: write of data block on socket failed.");
            free(answer);
            free(request);
            ret=0;
        }
        else{
            free(request);
            except_retrieve(cfd,answer,"os_store");
        }
    }
    else {
        if(!connected) fprintf(stderr,"os_store: not connected.\n");
        if(!name) printf("os_store: invalid name.\n");
        ret=0;
    }
    return ret;
}
void *os_retrieve(char *name){
    void* ret_v=NULL;
    if(name && connected) {
        int len=strlen("RETRIEVE  \n")+strlen(name);
        request=malloc(len);
        snprintf(request,len,"RETRIEVE %s \n",name);
        if(send(cfd,request,len,0)!=len) {
            perror("os_retrieve: ");
        }
        else{
            char* buf=malloc(MAX_BUFFSIZE);
            int red;
            if((red=s_readline(cfd,buf,MAX_BUFFSIZE))>0){
                if(!strncmp(buf,"DATA ",5)){
                    if(VERBOSE) {fprintf(stderr,"os_retrieve: complete message: ");fwrite(buf,1,red,stderr);fprintf(stderr,"\n");}
                    char* ptr,*tmp;
                    __strtok_r(buf," ",&ptr);
                    tmp=__strtok_r(NULL," ",&ptr);
                    int datalen=atoi(tmp);
                    if(VERBOSE) fprintf(stderr,"os_retrieve: message len=%d\n",datalen);
                    tmp=__strtok_r(NULL," ",&ptr);//ptr should now point to the start of the message "data" field
                    if(VERBOSE) {fprintf(stderr,"os_retrieve: message data:");fwrite(ptr,1,datalen,stderr);fprintf(stderr,"\n");}
                    ret_v=malloc(datalen);
                    memcpy(ret_v,ptr,datalen);
                    if(VERBOSE) {
                        fprintf(stderr,"os_retrieve: ");
                        if(ret_v==NULL) fprintf(stderr,"ret_v=NULL");
                        else fwrite(ret_v,1,datalen,stderr);
                        fprintf(stderr,"\n");
                    } 
                    free(buf);
                }
                else{
                    fprintf(stderr,"os_retrieve: communication protocol error.\n");
                }
            }
            else{
                fprintf(stderr,"os_retrieve: invalid read from socket.\n");
            }
        }
        free(request);
    }
    else {
        if(!connected) fprintf(stderr,"os_store: not connected.\n");
        if(!name) printf("os_store: invalid name.\n");
    }
    return ret_v;
}