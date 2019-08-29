#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "macros.h"
#include <assert.h>
#include "msg.h"
#include "support.h"
msg* tomsg(char* request,int fd){
    char* ptr;
    msg* req=malloc(sizeof(msg));
    req->name=NULL;
    if(request[strlen(request)-1]!='\n') {req->valid=0;fprintf(stderr,"Message doesn't end with newline.\n");return req;} 
    req->valid=1;
    char* token=__strtok_r(request," ",&ptr);
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
        fprintf(stderr,"Unknown command.\n");
        sendko(fd,"Unknown command.");
        return req;
    }

    token=__strtok_r(NULL," ",&ptr);
    if(token==NULL) { req->valid=0;fprintf(stderr,"Wrong command syntax.\n"); return req;}

    if(*token=='\n')//this is a LEAVE
        return req;
    else{//is a REGISTER|STORE|RETRIEVE|DELETE, storing NAME
        req->name=malloc(strlen(token)+1);
        strcpy(req->name,token);
    }

    token=__strtok_r(NULL," ",&ptr);
    if(token==NULL) {req->valid=0;fprintf(stderr,"Wrong command syntax.\n"); return req;}

    if(*token=='\n')//this is not a STORE, it doesn't have LEN field
        return req;
    else{//is a STORE, storing LEN
        req->len=strtol(token,NULL,10);
    }

    token=__strtok_r(NULL," ",&ptr);
    if(token==NULL || *token!='\n') { req->valid=0;fprintf(stderr,"Wrong command syntax.\n"); return req;}

    //read the one space character left in socket after '\n' to check if header is valid
    char space;
    if(readn(fd,&space,1)>0){
        if(space!=' ') {
            req->valid=0;
            if(VERBOSE){
                fprintf(stderr,"tomsg: store parsing, no space character found after newline. Character is ");
                if(space) fprintf(stderr,"-%c-\n",space);
                else fprintf(stderr,"invalid.\n");
            }
            return req;
        }
    }
    else{
        req->valid=0;
        if(VERBOSE) fprintf(stderr,"tomsg: Couldn't complete header parsing.");
    }
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
    }
}