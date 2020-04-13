#include "../include/server.h"

struct sockaddr_in serveraddr; //
int listenfd;                  //

struct epoll_event ev; //
struct epoll_event events[LISTENQ];
int epollfd; // epoll文件描述符

bool online;

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

std::string get_file_name (const int fd)
{
    if (0 >= fd) {
        return std::string ();
    }
 
    char buf[1024] = {'\0'};
    char file_path[100] = {'0'}; // PATH_MAX in limits.h
    snprintf(buf, sizeof (buf), "/proc/self/fd/%d", fd);
    if (readlink(buf, file_path, sizeof(file_path) - 1) != -1) {
        return std::string (file_path);
    }
 
    return std::string ();
}

Server::Server()
{
}

Server::~Server()
{
    close(listenfd);
    online = false;
    close(epollfd);
}

void Server::server_init()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0); //
    setnonblocking(listenfd);                   //把用于监听的socket设置为非阻塞方式

    bzero(&serveraddr, sizeof(serveraddr)); //设置服务器IP地址和端口号
    serveraddr.sin_family = AF_INET;
    inet_aton(HOSTIP, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(SERV_PORT); //或者htons(SERV_PORT);
    std::cout << "Server is listening in " << serveraddr.sin_addr.s_addr << ": " << serveraddr.sin_port << std::endl;

    epollfd = epoll_create(256);
    online = true;

    ev.data.fd = listenfd;                            //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;                    //设置要处理的事件类型
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); //注册epoll事件
    //std::cout<< "epoll path: "<<get_file_name(epollfd)<<std::endl;

    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
}
