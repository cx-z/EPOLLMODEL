#include "./include/events.h"

Event center;

void sig_handler( int sig )
{
    if ( sig == SIGINT)
    {
        center.online = false;
        close(center.listenfd);
        close(center.epollfd);
        exit(0);
    }
}


int main()
{
    signal( SIGINT, sig_handler );
    //signal( SIGINT, SIG_DFL );
    center.monitor();

    return 0;
}
