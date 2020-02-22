#include "../include/tasks.h"

TaskPool::TaskPool()
{
    //online = true;
    leftTasks = 0;
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
}

void TaskPool::exectask(int i)
{
    while(online)
    {   std::unique_lock<std::mutex> lk(threadmutex[i]);
        cv.wait(lk,[this]{
            return (leftTasks > 0);
        });
        Handler handler;
        handler.handle(*evs[i]);
        poptask();
        lk.unlock();
    }
   
}

void TaskPool::pushtask(struct epoll_event &hev, int i)
{
    //std::cout<< "ev address in TaskPool is"<< &ev<<std::endl;
    std::lock_guard<std::mutex> lock1(leftflag);
    std::lock_guard<std::mutex> lock2(threadmutex[i]);
    leftTasks++;
    evs[i]= &hev;
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