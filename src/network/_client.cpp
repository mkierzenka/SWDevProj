#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

int main(int argc, char * argv[])
{
    Client* c;

    //hardcoded value for now
    const char* servIp = "127.0.0.1";

    //first argument should be name of program
    //second argument flag and third value
    if (argc != 3)
    {
        fprintf(stderr, "Invalid number of arguments: %d\n", argc);
        exit(1);
    }

    const char* flag = argv[1];
    if (strcmp(flag, "-ip") == 0) {
        c = new Client(argv[2], servIp);
        c->registerSelf();
    } else {
        fprintf(stderr, "Invalid flag: %s", flag);
        exit(1);
    }


	while (true) {
		struct pollfd* fdList = c->getFDs();

		int rv = poll(fdList, 2 + c->clients_->length(), 300000);
		if (rv < 0) {
			fprintf(stderr,"Error while polling: %s\n",strerror(errno));
            return -1;
		} else if (rv == 0) {
			fprintf(stderr, "Client poller timeout!\n");
			return -1;
		}
		
		if ((fdList[0].revents&POLLIN) == POLLIN) {
			//server has data for me to read
			puts("Server has data to read");
			c->handleMessage(fdList[0].fd);
		} else if ((fdList[1].revents&POLLIN) == POLLIN) {
			//someone is talking on the fd where I listen for new conns
			puts("Client trying to talk to me");
			int newSock = c->acceptNewClient();
			c->handleMessage(newSock);
		}

		for (int i = 0; i < c->fdArr->length(); i++) {
			int idx = i+2; //get proper index for fdList
			
			if ((fdList[idx].revents&POLLIN) == POLLIN) {
				//a client wants to send me something		
				c->handleMessage(fdList[idx].fd);
			}
		}
	}


    delete c;
}