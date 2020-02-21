#include "../include/client.h"

Client::~Client()
{
    if (clifd > 0)
        close(clifd);
}

void Client::init_Client(struct sockaddr_in &addr, int id)
{
    cliaddr.sin_family = addr.sin_family;
    cliaddr.sin_port = addr.sin_port;
    cliaddr.sin_addr = addr.sin_addr;
    cliid = id;
}

int Client::init_link(struct sockaddr_in server)
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

void Client::send_request()
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

void Client::get_reply()
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

void Client::offline()
{
    online = false;
}
