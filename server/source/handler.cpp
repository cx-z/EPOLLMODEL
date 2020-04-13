#include "../include/handler.h"

#include <errno.h>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

Handler::Handler()
{
}

HandleError Handler::handle(struct epoll_event &hev){
    ssize_t n;
    if(hev.data.fd == listenfd) //监听事件
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
        epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev); //注册ev事件
    }
    else if(hev.events&EPOLLIN) //读事件
    {
        if ( (sockfd = hev.data.fd) < 0)
        {
            return READERR;
        }
        if ((n=read(sockfd, recvbuf, MAXLINE)) < 0) // 这里和IOCP不同
        {
            if(errno == ECONNRESET)
            {
                close(sockfd);
                hev.data.fd = -1;
            }
            else
            {
                std::cout<<"readline error"<<std::endl;
            }
        }
        else if (n == 0)
        {
            close(sockfd);
            hev.data.fd = -1;
        }
        ev.data.fd=sockfd; //设置用于写操作的文件描述符
        ev.events=EPOLLOUT | EPOLLET;  //设置用于注测的写操作事件 
        //修改sockfd上要处理的事件为EPOLLOUT
        epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
        printf("recv %s\n",recvbuf);
    }
    else if(hev.events&EPOLLOUT)//写事件
    {
        sockfd = hev.data.fd;
        write(sockfd, sendbuf, sizeof(sendbuf));
        ev.data.fd = sockfd; //设置用于读操作的文件描述符
        ev.events = EPOLLIN | EPOLLET; //设置用于注册的读操作事件
        //修改sockfd上要处理的事件为EPOLIN
        epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
    }
    return NORMAL;
}

/*HandleError Handler::handle(struct epoll_event &hev)
{
    if (hev.data.fd == listenfd) //监听事件
    {
        return elisten(hev);
    }
    else if (hev.events & EPOLLIN) //读事件
    {
        return eread(hev);
    }
    else if (hev.events & EPOLLOUT) //写事件
    {
        return ewrite(hev);
    }
}

HandleError Handler::elisten(struct epoll_event &hev)
{
    connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen);
    if (connfd < 0)
    {
        perror("connfd<0");
        return CONNECTERR;
    }
    setnonblocking(connfd); //把客户端的socket设置为非阻塞方式
    char *str = inet_ntoa(clientaddr.sin_addr);
    std::cout << "connect from " << str << std::endl;
    hev.data.fd = connfd;                            //设置用于读操作的文件描述符
    hev.events = EPOLLIN | EPOLLET;                  //设置用于注测的读操作事件
    epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &hev); //注册ev事件
    return NORMAL;
}
HandleError Handler::eread(struct epoll_event &hev)
{
    sockfd = hev.data.fd;
    if (sockfd < 0)
    {
        return READERR;
    }
    int n = read(sockfd, recvbuf, BUFFSIZE);
    if (n < 0) // 这里和IOCP不同
    {
        if (errno == ECONNRESET)
        {
            close(sockfd);
            hev.data.fd = -1;
        }
        else
        {
            std::cout << "readline error" << std::endl;
        }
    }
    else if (n == 0)
    {
        close(sockfd);
        hev.data.fd = -1;
    }
    hev.data.fd = sockfd;            //设置用于写操作的文件描述符
    hev.events = EPOLLOUT | EPOLLET; //设置用于注测的写操作事件 
    //修改sockfd上要处理的事件为EPOLLOUT
    epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &hev);
    printf("recv %s\n", recvbuf);
    return NORMAL;
}

HandleError Handler::ewrite(struct epoll_event &hev)
{
    sockfd = hev.data.fd;
    write(sockfd, sendbuf, strlen(sendbuf));
    hev.data.fd = sockfd;           //设置用于读操作的文件描述符
    hev.events = EPOLLIN | EPOLLET; //设置用于注册的读操作事件
    //修改sockfd上要处理的事件为EPOLIN
    epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &hev);
    return NORMAL;
}*/