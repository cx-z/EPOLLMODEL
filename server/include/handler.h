#include "server.h"

#define BUFFSIZE 100

enum TaskType
{
    LISTEN,
    READ,
    WRITE
};

enum HandleError
{
    NORMAL,
    CONNECTERR,
    READERR,
    WRITEERR
};

class Handler
{
public:
    Handler();
    HandleError handle(struct epoll_event &hev);
    HandleError eread(struct epoll_event &hev);
    HandleError ewrite(struct epoll_event &hev);
    HandleError elisten(struct epoll_event &hev);

private:
    struct sockaddr_in clientaddr;          //
    socklen_t clilen;                       //
    char recvbuf[BUFFSIZE];                 //
    char sendbuf[BUFFSIZE] = "I am server"; //
    int connfd;
    int sockfd;
};