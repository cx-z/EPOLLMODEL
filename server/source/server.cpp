#include "../include/server.h"

void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock, GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

Server::Server()
{
    online = true;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); //注册epoll事件

    bzero(&serveraddr, sizeof(serveraddr)); //设置服务器IP地址和端口号
    serveraddr.sin_family = AF_INET;
    inet_aton(HOSTIP, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(SERV_PORT); //或者htons(SERV_PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0); //
    setnonblocking(listenfd);                   //把用于监听的socket设置为非阻塞方式
    bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
    std::cout << "Server is listening in " << HOSTIP << ": " << serveraddr.sin_port << std::endl;

    ev.data.fd = listenfd;                            //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;                    //设置要处理的事件类型
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); //注册epoll事件

    std::cout << "Server's epollfd is " << epollfd << std::endl;
}

Server::~Server()
{
    close(listenfd);
    online = false;
    close(epollfd);
}
