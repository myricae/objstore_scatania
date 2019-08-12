#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>             
#include "htable.h"
#include "macros.h"
#define ERR_EXIT(s)\
    fprintf(stderr,s);\
    exit(EXIT_FAILURE)

unsigned int DJBHash(char* str, unsigned int len)
{
   unsigned long int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {   
      hash = ((hash << 5) + hash) + (*str);
   }  

   return hash;
}

node** init(node** ht){
    int i=0;
    ht=(node**)malloc(sizeof(node*)*HT_DIM);
    for(;i<HT_DIM;ht[i]=NULL,i++);
    return ht;
}
//rimuovere elem se non serve
int exists(char* name,node** ht){
    unsigned long int pos=DJBHash(name,strlen(name));
    pos=pos%HT_DIM;
    node* app=ht[pos];
    if(app==NULL) return 0;
    while(app!=NULL){
        if(!strcmp(app->name,name))
            return 1;
        app=app->next;
    }
    return 0;
}
int add(char* name,node** ht){
    if(ht==NULL) {ERR_EXIT("OS: Empty hash table.\n");}
    if(name==NULL || *name=='\0') {ERR_EXIT("OS: Invalid user name, exiting.\n");}
    node* new=(node*)malloc(sizeof(node));
    new->name=(char*)malloc(sizeof(char)*strlen(name)+1);
    strcpy(new->name,name);
    new->next=NULL;

    unsigned int pos=DJBHash(name,strlen(name));
    pos=pos%HT_DIM;

    if(VERBOSE) printf("hash position %d\n",pos);
    if(ht[pos]==NULL) ht[pos]=new;
    else{ //si potrebbe implementare in ordine alfabetico
        node* app=ht[pos];
        while(app->next!=NULL) app=app->next;
        app->next=new;
    }
    return pos;
}
int destroy(node** ht){
    int i=0;
    node* prec,*post;
    if(ht==NULL) return -1;
    for(;i<HT_DIM;i++){
        post=ht[i];
        if(post!=NULL){
            while(post!=NULL){
                prec=post;
                post=post->next;
                free(prec->name);
                free(prec);
            }
        }
    } 
    return 1;
}