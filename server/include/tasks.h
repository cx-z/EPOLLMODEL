#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

//#include "server.h"
#include "handler.h"

#define THREADNUM 20

class TaskPool: public Server
{
public:
    TaskPool();
    int getLeftTasksNum();
    void pushtask(struct epoll_event &tev, int i);
    void poptask();
    void exectask(int i);
    void start();
    void offline();
private:
    //int epollfd;
    std::vector<struct epoll_event*> evs;

    int leftTasks;
    std::mutex leftflag; // 锁定leftTasks
    std::mutex threadmutex[THREADNUM];
    std::condition_variable cv;
    std::vector<std::thread> handlerpool;
};