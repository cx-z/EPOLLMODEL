#include <vector>
#include <iostream>

#include "include/client.h"

std::vector<std::thread> clientPool;

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
