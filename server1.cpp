#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <csignal>

#define MAXLINE 100
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 30000
#define INFTIM 1000

void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(sock, GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

int listenfd, connfd, sockfd, epfd;

void sig_handler( int sig )
{
    if ( sig == SIGINT)
    {
        close(listenfd);
        close(connfd);
        close(sockfd);
        close(epfd);

    }
}


int main()
{
    //signal( SIGINT, sig_handler );
    signal( SIGINT, SIG_DFL );
    int i, maxi, nfds;
    ssize_t n;
    char recvMsg[MAXLINE];
    char sendMsg[] = "I am server";
    socklen_t clilen;
    //声明epoll_event结构体的变量, ev用于注册事件, events数组用于回传要处理的事件
    struct epoll_event ev,events[20];
    //生成用于处理accept的epoll专用的文件描述符, 指定生成描述符的最大范围为256 
    epfd = epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setnonblocking(listenfd); //把用于监听的socket设置为非阻塞方式
    ev.data.fd = listenfd; //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;  //设置要处理的事件类型
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev); //注册epoll事件
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    char local_addr[] = "127.0.0.1";
    inet_aton(local_addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(SERV_PORT); //或者htons(SERV_PORT);
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
    std::cout<<"Server is listening in "<<local_addr<<": "<<serveraddr.sin_port<<std::endl;
    maxi = 0;
    for( ; ; )
    {
        nfds = epoll_wait(epfd, events, 20, 500); //等待epoll事件的发生
        for(i = 0; i < nfds; ++i) //处理所发生的所有事件
        {
            if(events[i].data.fd == listenfd) //监听事件
            {
                connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen);
                if(connfd < 0)
                {
                    perror("connfd<0");
                    exit(1);
                }
                setnonblocking(connfd); //把客户端的socket设置为非阻塞方式
                char *str = inet_ntoa(clientaddr.sin_addr);
                std::cout << "connect from " << str <<std::endl;
                ev.data.fd=connfd; //设置用于读操作的文件描述符
                ev.events=EPOLLIN | EPOLLET; //设置用于注测的读操作事件
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); //注册ev事件
            }
            else if(events[i].events&EPOLLIN) //读事件
            {
                if ( (sockfd = events[i].data.fd) < 0)
                {
                    continue;
                }
                if ((n=read(sockfd, recvMsg, MAXLINE)) < 0) // 这里和IOCP不同
                {
                    if(errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        std::cout<<"readline error"<<std::endl;
                    }
                }
                else if (n == 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                ev.data.fd=sockfd; //设置用于写操作的文件描述符
                ev.events=EPOLLOUT | EPOLLET;  //设置用于注测的写操作事件 
                //修改sockfd上要处理的事件为EPOLLOUT
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
                printf("recv %s\n",recvMsg);
            }
            else if(events[i].events&EPOLLOUT)//写事件
            {
                sockfd = events[i].data.fd;
                write(sockfd, sendMsg, sizeof(sendMsg));
                ev.data.fd = sockfd; //设置用于读操作的文件描述符
                ev.events = EPOLLIN | EPOLLET; //设置用于注册的读操作事件
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
    close(listenfd);
    close(connfd);
    return 0;
}
