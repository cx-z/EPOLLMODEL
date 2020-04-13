#include <thread>

#include "../include/events.h"

void Event::monitor()  // 监听程序，启动服务器，启动事件池，循环监听
{
    server_init();
    taskpool.start();
    for (;;)
    {
        nfds = epoll_wait(epollfd, events, LISTENQ, 1000); //等待epoll事件的发生
        std::cout<<"发生事件数为："<<nfds<<std::endl;
        for (int i = 0; i < nfds; ++i) //处理所发生的所有事件
        {
            std::cout << i << std::endl;
            taskpool.pushtask(events[i], i);
        }
        if (nfds <= 0)
            continue;
        while (taskpool.getLeftTasksNum() > 0)
        {
            sleep(0.01);
        }
    }
}
