# httpServer
C++ http server using EPOLL

服务器端中心为事件Event，在主函数中定义一个Event变量，并调用Event成员函数server_init()初始化；
类Event中包含成员变量TaskPool，即任务池。Event每次通过epoll_wait监听到事件后，将事件event包装成task传入成员变量taskpool中。
类TaskPool中创建了LISTENQ个线程，即每次能监听到的最大事件数目，给每个线程分配一个事件。mutex数组中的元素与每个线程一一对应。
若某线程没有事件发生，则处于阻塞状态。有task传入时，对应的锁被函数pushtask释放。线程给对应的mutex加锁，调用函数Handler::handle()执行完毕后，将锁释放。
具体处理客户端请求的函数被封装在类Handler中。
