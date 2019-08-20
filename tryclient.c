#include "objstore.h"
#include <stdlib.h>
int main(int argc,char* argv[]){
    if(os_connect(argv[1])){
        printf("tryclient:registration succeeded.\n");
    }
    else{
        printf("tryclient:registration aborted.\n");
    }
    if(os_store("uno",(void*)"ad anna piacciono le peccore",29)){
        printf("tryclient:store succeeded\n");
    }
    else{
        printf("tryclient:store aborted\n");
    }
    if(os_delete("uno")){
        printf("tryclient:delete succeeded\n");
    }//provare con nome a cazzo
    else{
        printf("tryclient: delete aborted\n");
    }
    if(os_store("due",(void*)"ad anna piacciono le peccore",29)){
        printf("tryclient:store succeeded\n");
    }
    else{
        printf("tryclient:store aborted\n");
    }
    void* obj;
    if((obj=os_retrieve("due"))!=NULL){
        printf("tryclient: retrieve succeeded.\n");
    }
    else{
        printf("tryclient: retrieve aborted.\n");
    }
    char* due=(char*)obj;
    printf("retrieve result: ");
    if(due) printf("%s\n",due);
    else printf("NULL\n");
    
    if(os_disconnect()){
        printf("tryclient:disconnection succeeded.\n");
    }
    else{
        printf("tryclient:disconnection aborted.\n");
    }
    return 0;
}