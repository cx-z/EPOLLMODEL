#include "../include/tasks.h"

TaskPool::TaskPool()
{
    leftTasks = 0;
    evs = std::vector<struct epoll_event*>(THREADNUM);
    cv = std::vector<std::condition_variable>(THREADNUM);
}

int TaskPool::getLeftTasksNum()
{
    return leftTasks;
}

void TaskPool::start()
{
    for(int i=0;i<THREADNUM;i++)
        handlerpool.push_back(std::thread(&TaskPool::exectask,this, i));
    for(int i=0;i<handlerpool.size();i++)
        handlerpool[i].detach();
    std::cout<<"threadpool is ready"<<std::endl;
}

void TaskPool::exectask(int i)
{
    while(online)
    {   
        std::unique_lock<std::mutex> lk(threadmutex[i]);
        cv[i].wait(lk,[this]{
            return (leftTasks > 0);
        });
        printf("线程%d接收到事件。\n",i);
        Handler handler;
        handler.handle(*evs[i]);
        poptask();
        lk.unlock();
        printf("线程%d处理完事件。\n",i);
    }
   
}

void TaskPool::pushtask(struct epoll_event &hev, int i)
{
    {
        std::lock_guard<std::mutex> lock1(leftflag);
        std::lock_guard<std::mutex> lock2(threadmutex[i]);
        leftTasks++;
        evs[i]= &hev;
        printf("线程池给%d分配事件，leftTasks=%d。\n",i,leftTasks);
    }
    cv[i].notify_all();
}

void TaskPool::poptask()
{
    std::lock_guard<std::mutex> lock(leftflag);
    leftTasks--;
}

void TaskPool::offline()
{
    online = false;
}