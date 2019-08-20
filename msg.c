#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "macros.h"
#include "msg.h"
#include "support.h"
msg* tomsg(char* request,int fd){
    char* ptr;
    msg* req=malloc(sizeof(msg));
    req->data=req->name=NULL; 
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
        sendko(fd,"Unknown command.");
        return req;
    }

    if(req->com!=LEAVE){
        char* tmp=__strtok_r(NULL," ",&ptr);
        if(tmp==NULL) { req->valid=0;sendko(fd,"Wrong syntax."); return req;}
        req->name=malloc(strlen(tmp)+1);
        strcpy(req->name,tmp);
    }

    if(req->com!=STORE) return req;

    //Handling STORE command

    char* tmp=__strtok_r(NULL," ",&ptr);
    if(tmp==NULL) { req->valid=0;sendko(fd,"Wrong syntax."); return req;}
    int len=atoi(tmp);
    req->len=len;

    tmp=__strtok_r(NULL," ",&ptr);//ptr should now point to the start of the message "data" field
    req->data=malloc(len);
    memcpy(req->data,ptr,len);
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
        printf("(String) Data block: %s\n",message->data);
    }
    return;
}