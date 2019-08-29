#include "objstore.h"
#include "support.h"
#include <assert.h>

static int cfd=-1;
static int connected=0;
char answer[OKO_MAXLEN];

char* getdata(int datalen){
    char* databuf=malloc(sizeof(char)*(datalen+1));//+1 for the terminator \0, to allow the client to perform strlen(block)
    char space;
    if(readn(cfd,&space,1)!=1){
        if(VERBOSE)
            fprintf(stderr,"getdata: readn failed. Missing space after newline in retrieve answer.\n");
        return NULL;
    }
    if(space!=' '){
        if(VERBOSE)
            fprintf(stderr,"getdata: readn failed. Missing space after newline in retrieve answer.\n");
        return NULL;
    }
    if(readn(cfd,databuf,datalen)!=datalen){
        if(VERBOSE)
            fprintf(stderr,"getdata: readn failed.\n");
        return NULL;
    }
    databuf[datalen]='\0';
    return databuf;
}
int getko(char* answer,char* fun){
    if(!strncmp(answer,"KO ",3)){
        assert(((strlen(answer)-1)==strcspn(answer,"\n")));
        printf("%s",answer);
        return 0;
    }
    else{
        fprintf(stderr,"Irregular response from server.\n");
        return -1;
    }
}
//0: KO, 1:OK, -1:error retrieving answer.
int getoko(char* answer,char* fun,char** data){
    int red;
    if((red=getheader(cfd,answer,OKO_MAXLEN))>0){//in the RETRIEVE answer case I still have to read the space before the data block
        if(!strcmp(fun,"os_retrieve")){
            if(!strncmp(answer,"DATA ",5)){
                char* ptr;
                char* token=__strtok_r(answer+5," ",&ptr);
                if(token==NULL) {if(VERBOSE) fprintf(stderr,"%s: getoko: invalid header length field.\n",fun);return 0;}
                int datalen=strtol(token,NULL,10);
                int headerlen=strlen(answer)+2;//answer is tokenized (\0) right after LEN field
                if(VERBOSE) fprintf(stderr,"headerlen %d\n",headerlen);
                if(*(answer+headerlen-1)!='\n') {if(VERBOSE) fprintf(stderr,"%s: getoko: invalid message termination, missing newline. Character is: %c\n",fun,*(answer+headerlen-1));return 0;}
                char* datablock=getdata(datalen);
                if(datablock==NULL) return -1;
                else {*data=datablock;return 1;}
            }
        }
        else{
            if(!strncmp(answer,"OK \n",4)) {
                answer[red]='\0';
                printf("%s",answer);
                return 1;
            }
        }
        return getko(answer,fun);
    }
    else{
        fprintf(stderr,"%s: getheader failed. red %d bytes.\n",fun,red);
        return -1;
    }
}
    

int os_connect(char *name){
    if(!connected){
        struct sockaddr_un addr;      
        strcpy(addr.sun_path, SOCKNAME);
        addr.sun_family=AF_UNIX;
        ifmeno_usr(cfd=socket(addr.sun_family,SOCK_STREAM,0),"os_connect: problems creating socket");
        ifmeno_usr(connect(cfd,(struct sockaddr*)&addr,sizeof(addr)),"os_connect: problems with connect: ");
        connected=1;
        if(dprintf(cfd,"REGISTER %s \n",name)<0){
            fprintf(stderr,"os_connect: write on socket failed.\n");
            return 0;
        }
        return getoko(answer,"os_connect",NULL);
    }
    else {
        fprintf(stderr,"os_connect: already connected.\n");
        return 0;
    }
}

int os_disconnect(){
    if(connected){
        if(dprintf(cfd,"LEAVE \n")<0){
            fprintf(stderr,"os_disconnect: write on socket failed.\n");
            return 0;
        }
        connected=0;
        int ret=getoko(answer,"os_disconnect",NULL);
        close(cfd);
        return ret;
    }
    else{
        fprintf(stderr,"os_disconnect: client needs to call os_connect first.\n");
        return 0;
    }
}
int os_delete(char *name){
    if(name && connected) {
        if(dprintf(cfd,"DELETE %s \n",name)<0) {
            fprintf(stderr,"os_delete: write on socket failed.\n");
            return 0;
        }
        else
            return getoko(answer,"os_delete",NULL);
    }
    else {
        if(!connected) printf("os_delete: not connected.\n");
        if(!name) printf("os_delete: invalid name.\n");
        return 0;
    }
}
int os_store(char *name, void *block, size_t len){
    if(name && connected &&len>=1 && block!=NULL) {
        if(dprintf(cfd,"STORE %s %zu \n ",name,len)<0){
            fprintf(stderr,"os_store: write of header on socket failed.\n");
            perror("os_store: dprintf: ");
            return 0;
        }

        if(writen(cfd,(char*)block,len)<0){
            fprintf(stderr,"os_store: write of data block on socket failed.\n");
            return 0;
        }
        else
            return getoko(answer,"os_store",NULL);
    }
    else {
        if(block==NULL) fprintf(stderr,"os_store: invalid block.\n");
        if(len<1) fprintf(stderr,"os_store: invalid block len.\n");
        if(!connected) fprintf(stderr,"os_store: not connected.\n");
        if(!name) fprintf(stderr,"os_store: invalid name.\n");
        return 0;
    }
}
void *os_retrieve(char *name){
    char* ret_v=NULL;
    if(name && connected) {
        if(dprintf(cfd,"RETRIEVE %s \n",name)<0) {
            perror("os_retrieve: ");
            return NULL;
        }
        else{
            int ret;
            if((ret=getoko(answer,"os_retrieve",&ret_v))==1)
                return (void*)ret_v;
            if(VERBOSE) fprintf(stderr,"os_retrieve: getoko returned %d\n",ret);
            else return NULL;
        }
    }
    else {
        if(!connected) fprintf(stderr,"os_store: not connected.\n");
        if(!name) printf("os_store: invalid name.\n");
        return NULL;
    }
    return NULL;
}