typedef enum _cmd {REGISTER,STORE,RETRIEVE,DELETE,LEAVE} cmd;
typedef struct _msg{
    cmd com;
    char* name;
    unsigned int len;
    char* data;
    int valid;
}msg;
msg* tomsg(char* request,int fd,int* header_len);
void printmsg(msg* message);