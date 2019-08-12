void sendok(int fd);
void sendko(int fd,char* msg);
void sendok_retr(int fd,int len,char* data);
int s_readline(int fd, char *buf,int capacity);
typedef struct _client_t{
    int fd;
    int on;
    int reg;
    char* name;
    char* path;
    //some more
}client_t;