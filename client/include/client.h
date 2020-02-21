#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <time.h>

#define MAXLINE 100

class Client
{
public:
    Client() {}
    ~Client();
    void init_Client(struct sockaddr_in &addr, int id);
    int init_link(struct sockaddr_in server);
    void send_request();
    void get_reply();
    void offline();

private:
    int cliid;                         // 客户ID
    int clifd;                         // 客户端套接字文件描述符
    char recvMsg[MAXLINE];                 // 接收缓存
    char sendMsg[MAXLINE] = "I am client"; // 发送内容缓存
    struct sockaddr_in cliaddr;        // 客户端地址
    bool online = false;
};
