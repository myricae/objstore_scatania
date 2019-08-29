int sendok(int fd);
int sendko(int fd,char* msg);
int sendok_retr(int fd,int len,FILE* f);
int getheader(int fd,char* buf,int maxlen);
int waitdata(int fd);
int writen(int fd,char* buf,int n);
int readn(int fd,char* buf,int n);
typedef struct _client_t{
    int fd;
    int on;
    int reg;
    char* name;
    char* path;
    //some more
}client_t;