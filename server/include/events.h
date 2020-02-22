#include <thread>

//#include "server.h"
#include "tasks.h"

class Event: public Server
{
public:
    Event();
    void monitor();
    void pushtask(struct epoll_event &ev);
private:

    int nfds; // 活跃的事件数量
    //声明epoll_event结构体的变量,events数组用于回传要处理的事件
    struct epoll_event events[LISTENQ];

    TaskPool taskpool;
};