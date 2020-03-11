#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#include "server.h"

int main(int argc, char *argv[])
{
    Server *s;
    int TIMEOUT = 15000; //timeout in milliseconds
    //first argument should be name of program
    //second argument flag and third value
    if (argc != 3)
    {
        fprintf(stderr, "Invalid number of arguments: %d\n", argc);
        exit(1);
    }

    const char *flag = argv[1];
    if (strcmp(flag, "-ip") == 0)
    {
        s = new Server(argv[2]);
    }
    else
    {
        fprintf(stderr, "Invalid flag: %s", flag);
        exit(1);
    }

    while (true)
    {
        struct pollfd *listenFd = s->getPollObj();
        int pollVal = poll(listenFd, 1, TIMEOUT);
        if (pollVal < 0)
        {
            fprintf(stderr, "Error while polling: %s\n", strerror(errno));
            return -1;
        }
        else if (pollVal == 0)
        {
            printf("Initiating teardown\n");
            s->initiateTeardown();
            exit(1);
            return -1;
        }

        if ((listenFd[0].revents & POLLIN) == POLLIN)
        {
            s->acceptNewClient();
            s->handleMessage();
        }
    }

    delete s;
}
