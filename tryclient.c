#include "objstore.h"
#include <stdlib.h>
int main(int argc,char* argv[]){
    if(os_connect(argv[1])){
        printf("tryclient:registration succeeded.\n");
    }
    else{
        printf("tryclient:registration aborted.\n");
    }
    /*
    if(os_store("Obj1",(void*)"ad anna piacciono le peccore",29)){
        printf("tryclient:store succeeded\n");
    }
    else{
        printf("tryclient:store aborted\n");
    }
    if(os_delete("Obj1")){
        printf("tryclient:delete succeeded\n");
    }//provare con nome a cazzo
    else{
        printf("tryclient: delete aborted\n");
    }*/
    
    char* ogg="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent nulla velit, efficitur in ex vehicula, consequat condimentum sem. Vivamus malesuada pharetra erat, a aliquam massa iaculis vel. Pellentesque vehicula ac erat nec imperdiet. Maecenas sit amet condimentum magna, nec bibendum dui. Fusce scelerisque dui est. Nam quam eros, tincidunt vel ipsum sed, fringilla hendrerit eros. Nam ultrices interdum odio sed dapibus. Aenean eu commodo ante. Ut vel consectetur urna. Etiam eros sapien, pretium sed leo ac, posuere facilisis dui. Donec sit amet accumsan eros.\n\nCras eget elit rutrum, accumsan dolor nec, sollicitudin urna. Cras at nisl vulputate, interdum ex id, tincidunt lacus. Integer ut tempus mauris. Vivamus accumsan, dui vitae luctus pretium, turpis eros pulvinar urna, non iaculis leo odio in nunc. Nullam varius ipsum ut justo pulvinar tincidunt. Quisque vitae gravida justo, a dignissim lorem. Aliquam sed sagittis nibh. Integer commodo rhoncus dui eu tristique. Fusce tristique ligula eget metus pulvinar vulputate. Vestibulum imperdiet justo ut ante sollicitudin mollis. Duis non nibh feugiat, imperdiet lacus et, ultricies risus. Quisque rutrum aliquam tempor. Phasellus velit mi, efficitur at arcu ut, porttitor pharetra nisi. Sed quis dolor tristique, pellentesque ipsum eget, dignissim nisi. Nunc aliquet nunc sapien, at dapibus nibh interdum vel. Etiam elementum molestie ligula, sit amet porttitor ex lobortis ornare.\n\nCras massa leo, efficitur ut gravida ut, porta id nulla. Maecenas pellentesque, sem ut sodales pulvinar, turpis tellus semper lectus, aliquet condimentum ligula sapien sed lacus. Etiam vulputate neque a ligula congue sagittis. Donec fermentum maximus purus eu aliquet. Proin volutpat eleifend risus, eu porta sapien tincidunt vel. Integer lectus sapien, consectetur in dignissim eget, bibendum nec nulla. Praesent efficitur lectus vel iaculis tincidunt. Fusce a erat non nisi facilisis pulvinar. Proin ultrices feugiat nibh vitae tempor. Vestibulum dignissim lorem nec gravida pharetra. Nam egestas magna scelerisque, molestie nunc vel, luctus metus. Sed ac arcu magna. In hac habitasse platea dictumst. Vivamus tristique leo in lorem auctor mattis.\n\nMaecenas quam lacus, facilisis eu odio interdum, sodales convallis ex. Cras vitae tellus eu sem ultrices consequat at et felis. Donec tempus nunc vitae justo bibendum viverra. Fusce efficitur ac metus in vestibulum. Duis sagittis non lectus semper pulvinar. Suspendisse eleifend sapien ligula, non mollis diam blandit eu. Curabitur pretium, orci bibendum cursus congue, tortor neque molestie risus, sit amet cursus diam ante in sem. Quisque posuere consequat turpis vestibulum pellentesque. Maecenas placerat, dui non lobortis ornare, risus lorem rhoncus odio, in consectetur augue nunc ut dolor. Aliquam erat volutpat. Quisque id pretium turpis. Fusce ornare nulla quis nisl aliquam, dignissim euismod sapien consequat. Aenean vehicula ante pellentesque, volutpat nisl et, porta neque. Sed commodo dui quis lectus facilisis hendrerit.\n\nMaecenas interdum lorem ut tempor varius. Curabitur vitae facilisis ante. Vivamus vel gravida tellus. Vivamus diam libero, maximus et eros in, interdum ultricies tortor. Proin consectetur ex eu leo tempus, at consectetur lectus auctor. In auctor libero ac mi facilisis mollis. Mauris feugiat eros nec neque tristique, sit amet gravida tellus venenatis. Etiam vel lectus blandit lorem venenatis pellentesque et at orci. Interdum et malesuada fames ac ante ipsum primis in faucibus.\n\nInteger volutpat ante nibh, sit amet blandit libero laoreet sit amet. Nulla vitae magna at diam venenatis sagittis. Proin eleifend fringilla mauris. Proin commodo rhoncus massa in accumsan. Donec venenatis ipsum nec ligula pellentesque congue. Etiam luctus euismod nisi, id consequat tortor interdum porta. Curabitur pellentesque, massa ac dictum interdum, lacus elit elementum felis, in facilisis ante ipsum a erat. Quisque a erat sed lacus ultrices consectetur id at dolor. Nullam diam lorem, faucibus nec imperdiet non, feugiat vitae ex. Pellentesque laoreet quam vitae tempor lobortis. In id ante felis. Etiam at vehicula orci. Morbi mollis, lacus sit amet congue euismod, sapien ex dictum mi, sit amet accumsan ligula ante in quam. Morbi mattis vel dui ac auctor. Morbi eu eros vitae nisl ultrices dignissim nec ut nisi.\n\nPhasellus in nulla id sem finibus tristique id eget ante. Sed ac justo sem. Vestibulum pretium tellus vel facilisis commodo. Curabitur laoreet euismod sapien eget condimentum. Donec porta nisl in libero dignissim, eu dapibus ipsum ultricies. Aliquam a posuere arcu. Donec et ullamcorper erat. Duis sed mollis neque, vulputate tristique odio. Etiam quis tellus sed ante hendrerit tempor iaculis id nisi. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Donec ac vehicula metus. Phasellus auctor tellus vitae suscipit eleifend. Curabitur venenatis dolor nec lorem vestibulum, luctus congue turpis rutrum. Aenean ultricies lacus at augue finibus porttitor. Praesent sodales feugiat leo in porta.\n\nVivamus nibh tellus, suscipit varius erat nec, volutpat mollis massa. Nam nibh lorem, aliquet et odio in, elementum euismod neque. Nullam sed arcu bibendum, iaculis augue non, facilisis mi. Maecenas iaculis, nisi et feugiat rutrum, nibh lectus rhoncus mauris, ac rhoncus urna neque quis risus. Quisque blandit lectus sed lacinia aliquam. Maecenas dapibus lacus eget pellentesque finibus. Nullam ultrices consectetur ante at dapibus. Nam dapibus molestie scelerisque.\n\nEtiam condimentum condimentum neque. Maecenas nec sodales enim, ut accumsan dui. Vivamus vel lectus sit amet neque sollicitudin imperdiet. Vestibulum fringilla libero mollis leo ullamcorper, eget congue orci molestie. Etiam aliquam, ipsum tincidunt imperdiet tincidunt, augue quam congue erat, ac consequat sem turpis in dui. Mauris ultricies erat at urna hendrerit, at convallis sem venenatis. Morbi a lorem condimentum, vestibulum massa in, gravida nulla. Praesent sodales aliquam imperdiet. Donec eu tincidunt nunc, vitae rutrum sem. Donec ultricies ante ac congue dapibus. Sed posuere quis risus vitae aliquet. Maecenas ac molestie lectus, vel scelerisque quam. Sed congue magna quis fringilla pharetra. Nunc et bibendum felis. Proin vitae lacus eget sem rutrum sodales. Donec tellus libero, euismod eu laoreet et, accumsan a lorem.\n\nCras venenatis mattis pharetra. Maecenas eu faucibus ante. Nulla tristique rutrum eros sed pellentesque. Aenean finibus metus vitae justo luctus vulputate. Aenean lobortis mauris quis arcu tempor, nec rhoncus magna vestibulum. Donec aliquam consectetur finibus. Praesent laoreet ac libero vitae cursus. Nulla at erat interdum, euismod augue sit amet, mattis neque. Nunc sed pellentesque nibh. Phasellus vitae ornare est, a facilisis turpis. Pellentesque in tincidunt augue. Donec imperdiet est nunc, eu dignissim libero sagittis quis. Sed tincidunt vulputate placerat. Proin molestie dictum nisi, sit amet gravida justo varius ut.\n\nDonec congue dolor semper enim cursus sodales non a dolor. Aenean at hendrerit augue. Proin vitae commodo ipsum. Proin ullamcorper sem at tellus pulvinar pellentesque. Nullam convallis, metus interdum vulputate hendrerit, augue enim eleifend risus, sed fermentum massa orci sed lacus. Proin ullamcorper ipsum sit amet nisl efficitur ornare. Nullam consequat tellus augue, a facilisis enim faucibus eget. Praesent tellus nisl, sodales eget euismod eu, commodo vel turpis. Ut tempus velit vel quam condimentum vehicula. Sed feugiat turpis purus, sed pharetra tellus cursus ut. Donec at mollis nisi, vitae volutpat erat. Suspendisse ut varius mi. Vivamus erat dui, fermentum vel sollicitudin nec, vestibulum sit amet sapien.\n\nUt a nunc vel sem tincidunt tempor. Nulla scelerisque dui enim, et laoreet lorem dignissim quis. Maecenas ultrices felis auctor lacinia volutpat. Praesent imperdiet urna magna, eget mollis turpis lacinia sit amet. Phasellus congue ipsum ut nunc commodo sollicitudin vitae sed mi. Proin commodo dui in odio tempus laoreet. Morbi faucibus lacinia elit nec sagittis. Curabitur massa neque, commodo sed tortor sit amet, feugiat imperdiet ipsum. Duis mattis vestibulum dui, vitae consectetur lorem cursus eget. Suspendisse blandit vel justo non fermentum. Cras pulvinar urna ante, ac malesuada ante rhoncus at. Duis eu tempus nunc. Mauris quis dui metus. Cras sit amet pellentesque diam. Maecenas nec massa nec nisl tincidunt congue sed ut nunc.\n\nDonec arcu ligula, elementum non feugiat eu, finibus nec mauris. Etiam dictum, ligula eget finibus aliquet, ex nisi consectetur enim, at condimentum mauris ante ac dui. Curabitur sodales, dui ac molestie lacinia, leo enim maximus tortor, non interdum lectus est nec ipsum. Pellentesque sed posuere odio. In sed ipsum mollis, imperdiet mauris egestas, varius magna. Sed eu dolor quis neque feugiat laoreet sed volutpat sem. Nullam volutpat laoreet libero, non dapibus lectus. Phasellus euismod dictum ante, vitae elementum tellus placerat sit amet. Proin semper nisi elit, eu pharetra ipsum porta sit amet. Fusce ac porttitor mi, nec volutpat justo. Vivamus sit amet vestibulum nunc. Integer non mauris metus. Ut condimentum elit mauris, in sodales mauris pellentesque in. Proin non efficitur urna, vehicula feugiat eros. Quisque et dui id lacus commodo sodales quis et arcu. Nulla at lacus est.\n\nVestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Nunc augue ipsum, condimentum quis orci ac, porttitor congue lacus. Suspendisse ut elit sit amet magna aliquam consectetur vitae fermentum ante. Praesent eget tortor iaculis erat faucibus faucibus elementum et turpis. Morbi vitae nulla quis orci ultricies consequat a non velit. Suspendisse congue eget metus placerat dictum. Proin tempor, leo in pharetra eleifend, elit magna faucibus ex, ut imperdiet ipsum nisi id enim. Nulla sed sodales nibh, quis suscipit augue. Sed ultrices arcu ac lorem vehicula semper. Donec at urna cras amet. ";
    /*int i=0;
    ogg[0]='\0';
    for(;i<100;i++){
        strcat(ogg,"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent iaculis arcu eu tempor cras amet.");
    }*/
    
    //printf("tryclient: test file content: \n");
    //printf("dati scritti %s",ogg);

    if(os_store("Obj2",(void*)ogg,10000)){
        printf("tryclient:store succeeded\n");
    }
    else{
        printf("tryclient:store aborted\n");
    }
    /*void* obj;
    if((obj=os_retrieve("due"))!=NULL){
        printf("tryclient: retrieve succeeded.\n");
    }
    else{
        printf("tryclient: retrieve aborted.\n");
    }

    printf("tryclient retrieve result: \n");
    printf("         : void object: ");
    fwrite(obj,1,28,stdout);
    printf("\n");
    fflush(stdout);

    char* due=(char*)obj;
    printf("char object: ");
    if(due) printf("%s\n",due);
    else printf("NULL\n");
    int size;
    if((size=strlen("ad anna piacciono le peccore"))==sizeof(obj)){
        printf("dimensione uguale.\n");
        if(!memcmp((void*)"ad anna piacciono le peccore",obj,size)) printf("stringhe uguali.\n");
        else printf("stringhe diverse.");
    }   */
    if(os_disconnect()){
        printf("tryclient:disconnection succeeded.\n");
    }
    else{
        printf("tryclient:disconnection aborted.\n");
    }
    free(ogg);
    return 0;
}