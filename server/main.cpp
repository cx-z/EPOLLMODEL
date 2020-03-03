#include "./include/events.h"

Event center;

void sig_handler( int sig )
{
    if ( sig == SIGINT)
    {
        online = false;
        close(listenfd);
        close(epollfd);
        exit(0);
    }
}


int main()
{
    //signal( SIGINT, sig_handler );
    signal( SIGINT, SIG_DFL );
    //center.server_init();
    center.monitor();

    return 0;
}