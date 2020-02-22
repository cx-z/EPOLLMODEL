#include <thread>

#include "../include/events.h"

Event::Event()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0); //
    setnonblocking(listenfd);                   //把用于监听的socket设置为非阻塞方式
    bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
    std::cout << "Server is listening in " << HOSTIP << ": " << serveraddr.sin_port << std::endl;

    ev.data.fd = listenfd;                            //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;                    //设置要处理的事件类型
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); //注册epoll事件

    nfds = 0;
    std::cout << "Event's epollfd is " << epollfd << std::endl;
}

void Event::monitor()
{
    taskpool.start();
    int cnt = 0;
    for (;;)
    {
        nfds = epoll_wait(epollfd, events, 20, 500); //等待epoll事件的发生
        //std::cout<<"nfds: "<<nfds<<std::endl;
        for (int i = 0; i < nfds; ++i)               //处理所发生的所有事件
        {
            cnt++;
            taskpool.pushtask(events[i], i);
        }
        if(cnt == 5)
            std::cout<<"有连接被epoll捕捉到\n"<<std::endl;
        if(nfds <= 0)
            continue;
        while (taskpool.getLeftTasksNum() > 0)
        {
            sleep(0.01);
        }
    }
}