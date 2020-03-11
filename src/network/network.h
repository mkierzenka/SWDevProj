#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>

/** This class wraps the basic functionality of the POSIX libraries.
 * It is used to make connections between clients and the server. */
class Network : public Object
{

public:
    int fd, newSock; //file descriptor of the sockets
    int opt = 1;
    struct sockaddr_in address;
    int addresslen = sizeof(address);
    int PORT = 8080;

    /**
     * Network constructor. Will take string IP of the client/server
     * that it was initialized by. */
    Network(const char* addr)
    {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(fd > 0);
        //hardcode options for now
        int rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        assert(rc >= 0);

        address.sin_family = AF_INET;
        int inet = inet_pton(AF_INET, addr, &address.sin_addr);
        assert(inet > 0);
        address.sin_port = htons(8080);
    }

    /** Binds bock to address and corresponding port number.
     * Only needed by socket.*/
    void bindSocket()
    {
        int val = bind(fd, (struct sockaddr *)&address,
                       sizeof(address));

        if (val < 0) {
            fprintf(stderr, "ERRNO: %d\n", errno);
        }
        assert(val >= 0);
        printf("Server binded to socket\n");
    }

    /** Allows server to wait for client to make connection. */
    void listenForClient(int fileDesc)
    {
        //second parameter doesn't seem like a huge deal
        int val = listen(fileDesc, 32);

        assert(val >= 0);
        printf("Server listening for client...\n");
    }

    /** Pulls first connection requests off queue of requests.
     * Will only be used by server */
    int acceptConnection(int fileDesc)
    {
        newSock = accept(fileDesc, (struct sockaddr *)&address,
                      (socklen_t *)&addresslen);
                      
        assert(newSock >= 0);
        printf("Client accepted!\n");

        return newSock;
    }

    /** Attempt to connect to server's socket. Only used by client. */
    void connectToServer()
    {
        //printf("Attempting to connect to server...\n");
        int connection = connect(fd, (struct sockaddr *)&address,
                       sizeof(address));

        assert(connection >= 0);
        //printf("Server connected!\n");
    }

    /** Send a message to the given IP with the given contents. */
    void sendMessage(const char* msg, int fileDesc)
    {
        int sendVal = send(fileDesc, msg, strlen(msg), 0);
        assert(sendVal >= 0);
        printf("Sent message contents: %s\n", msg);
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    char* receiveMessage(int fileDesc)
    {
        //create buffer for message
        char* buffer = new char[1024];
        memset(buffer, 0, 1024);

        int valread = read(fileDesc, buffer, 1024);
        assert(valread >= 0);
        assert(buffer != NULL);
        return buffer;
    }


    /** Get fd for server's socket */
    int getServerFd() {
        return fd;
    }


};
