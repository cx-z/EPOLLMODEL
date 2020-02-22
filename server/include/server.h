#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define MAXLINE 100
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 30000
#define INFTIM 1000
#define HOSTIP "127.0.0.1"

void setnonblocking(int sock);

/*struct sockaddr_in serveraddr; //
int listenfd; //

struct epoll_event ev; // 
int epollfd; // epoll文件描述符

bool online;*/

class Server
{
public:
    Server();
    ~Server();
    void server_init();
public:
    struct sockaddr_in serveraddr; //
    int listenfd; //

    struct epoll_event ev; // 
    int epollfd; // epoll文件描述符

    bool online;
};
