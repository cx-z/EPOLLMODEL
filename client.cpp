#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <time.h>
#include <iostream>

#define MAXLINE 100

std::vector<std::thread> clientPool;

class Client
{
public:
    Client() {}
    ~Client()
    {
        if (clifd > 0)
            close(clifd);
    }
    void init_Client(struct sockaddr_in &addr, int id)
    {
        cliaddr.sin_family = addr.sin_family;
        cliaddr.sin_port = addr.sin_port;
        cliaddr.sin_addr = addr.sin_addr;
        cliid = id;
    }
    int init_link(struct sockaddr_in server)
    {
        if ((clifd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
            return -1;
        }
        int state = connect(clifd, (struct sockaddr *)&server, sizeof(server));
        if (state < 0)
        {
            printf("client%d can't connect to the server\n", cliid);
            return -1;
        }
        else
        {
            online = true;
            printf("client%d has connected to the server\n", cliid);
            return 0;
        }
    }
    void send_request()
    {
        while (online)
        {
            if (send(clifd, sendMsg, sizeof(sendMsg), 0) < 0)
            {
                printf("client%d send msg error: %s(errno: %d)\n", cliid, strerror(errno), errno);
            }
            else
            {
                printf("client%d sent msg \"%s\" succeed.\n", cliid, sendMsg);
            }
            sleep(2);
        }
    }
    void get_reply()
    {
        while (online)
        {
            //printf("get_reply\n");
            int recvLen = recv(clifd, recvMsg, sizeof(recvMsg), 0);
            if (recvLen < 0)
            {
                printf("client%d recv msg error: %s(errno: %d)\n", cliid, strerror(errno), errno);
                if (errno != EINTR)
                    online = false;
            }
            else
            {
                printf("client%d recv msg \"%s\" succeed.\n", cliid, recvMsg);
            }
            sleep(2);
        }
    }
    void communicate()
    {
        //std::thread sendThread(&Client::send_request, NULL);
        //std::thread recvThread(&Client::get_reply, NULL);
        //sendThread.join();
        //recvThread.join();
    }
    void offline()
    {
        online = false;
    }

private:
    int cliid;                         // 客户ID
    int clifd;                         // 客户端套接字文件描述符
    char recvMsg[100];                 // 接收缓存
    char sendMsg[100] = "I am client"; // 发送内容缓存
    struct sockaddr_in cliaddr;        // 客户端地址
    bool online = false;
};

int main()
{
    std::vector<Client> clients;
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(30000);
    char serverIP[] = "127.0.0.1";
    inet_aton(serverIP, &(serveraddr.sin_addr));
    std::cout<<"Server is listening in "<< serverIP <<": "<<serveraddr.sin_port<<std::endl;
    struct sockaddr_in clientaddr;
    clientaddr.sin_family = AF_INET;
    int cport = 40000;

    clientaddr.sin_port = htons(cport);
    for (int i = 0; i < 3; i++)
    {
        clients.push_back(Client());
        clientaddr.sin_port = htons(cport++);
        clients[i].init_Client(clientaddr, i);
        if (clients[i].init_link(serveraddr) < 0)
            return 1;
    }
    for (int i = 0; i < clients.size(); i++)
    {
        //clientPool.push_back(std::thread(&Client::communicate, &clients[i]));
        clientPool.push_back(std::thread(&Client::send_request, &clients[i]));
        clientPool.push_back(std::thread(&Client::get_reply, &clients[i]));
    }
    for (int i = 0; i < clientPool.size(); i++)
    {
        clientPool[i].detach();
    }
    sleep(30);
    for (int i = 0; i < clients.size(); i++)
        clients[i].offline();
    return 0;
}