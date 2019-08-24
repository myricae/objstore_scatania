#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "macros.h"
#include "msg.h"
#include "support.h"
msg* tomsg(char* request,int fd,int * header_len){
    char* ptr;
    msg* req=malloc(sizeof(msg));
    req->data=req->name=NULL; 
    req->valid=1;
    int com_len,len_len,name_len;
    char* token=__strtok_r(request," ",&ptr);
    com_len=strlen(token);
    if(!strcmp(token,"REGISTER")){
        req->com=REGISTER;
    }
    else if(!strcmp(token,"LEAVE")){
        req->com=LEAVE;
    }
    else if(!strcmp(token,"STORE")){
        req->com=STORE;
    }
    else if(!strcmp(token,"RETRIEVE")){
        req->com=RETRIEVE;
    }
    else if(!strcmp(token,"DELETE")){
        req->com=DELETE;
    }
    else {
        req->valid=0;
        sendko(fd,"Unknown command.");
        return req;
    }

    char* tmp;

    if(req->com!=LEAVE){
        tmp=__strtok_r(NULL," ",&ptr);
        if(tmp==NULL) { req->valid=0;sendko(fd,"Wrong syntax."); return req;}
        req->name=malloc(strlen(tmp)+1);
        strcpy(req->name,tmp);
        name_len=strlen(req->name);
    }

    if(req->com!=STORE) return req;

    //Handling STORE command

    tmp=__strtok_r(NULL," ",&ptr);
    if(tmp==NULL) { req->valid=0;sendko(fd,"Wrong syntax."); return req;}
    len_len=strlen(tmp);
    req->len=atoi(tmp);;

    *header_len=com_len+name_len+len_len+5;//5= 1 '\n' + 4 ' ' (+1 after STORE,+1 after NAME,+1 after DATALEN & +1 after '\n')
    tmp=__strtok_r(NULL," ",&ptr);//ptr should now point to the start of the message "data" field
    int residue=MAX_BUFFSIZE-*header_len;
    if(residue>req->len){
        req->data=calloc(req->len,sizeof(char));
        memcpy(req->data,tmp+2,req->len);
        if(VERBOSE){ fprintf(stderr,"Allocating directly in req->data: ");
        fwrite(req->data,1,req->len,stderr);
        fprintf(stderr,"\n");
        }
    }
    else req->data=ptr;
    return req;
}
void printmsg(msg* message){
    printf("Command: ");
    switch(message->com){
        case REGISTER:
            printf("REGISTER\n");
            break;
        case STORE:
            printf("STORE\n");
            break;
        case RETRIEVE:
            printf("RETRIEVE\n");
            break;
        case DELETE:
            printf("DELETE\n");
            break;
        case LEAVE:
            printf("LEAVE\n");
            break;
        default:
            printf("Unknown.\n");
            return;
    }

    if(message->com!=LEAVE){
        printf("Name: %s\n",message->name);
        if(message->com!=STORE) return;
        printf("Length: %u\n",message->len);
        printf("(String) Data block: ");
        fwrite(message->data,1,sizeof(message->data),stdout);
        printf("\n");
    }
    return;
}