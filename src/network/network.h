#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "../../utils/thread.h"
#include "message.h"
#include "msgkind.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <assert.h>

/** This class wraps the basic functionality of the POSIX libraries.
 * It is used to make connections between clients and the server. */
class Network : public Object
{

public:
    Serializer* s; //used for sending and receiving message
    Lock lock_; //lock for reading from and writing to serializer
    int fd; //descriptor to send message to this node
    size_t nodeId_; //id of node
    int opt = 1;
    struct sockaddr_in address;
    int addresslen = sizeof(address);
    int PORT = 8080;

    /**
     * Network constructor. Will take string IP of the client/server
     * that it was initialized by. */
    Network(const char* addr, size_t nodeId)
    {
        s = new Serializer();
        nodeId_ = nodeId;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(fd > 0);
        //hardcode options for now
        int rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        assert(rc >= 0);

        address.sin_family = AF_INET;
        int inet = inet_pton(AF_INET, addr, &address.sin_addr);
        assert(inet > 0);
        address.sin_port = htons(8080);
        bindSocket_();
    }

    ~Network()
    {
        delete s;
    }

    /** Allows node to wait for other nodes to make connection to it */
    void listenForConnections()
    {
        //second parameter doesn't seem like a huge deal
        int val = listen(fd, 32);

        assert(val >= 0);
        printf("Node %zu listening for connections...\n", nodeId_);
    }

    /** Pulls first connection requests off queue of requests.
     * Will only be used by server */
    int acceptConnection()
    {
        int newSock = accept(fd, (struct sockaddr *)&address,
                      (socklen_t *)&addresslen);
                      
        assert(newSock >= 0);
        printf("New node accepted!\n");

        return newSock;
    }

    /** Attempt to connect to socket with given fd */
    void connectToNode(int fileDesc)
    {
        //printf("Attempting to connect to server...\n");
        int connection = connect(fileDesc, (struct sockaddr *)&address,
                       sizeof(address));

        assert(connection >= 0);
        //printf("Server connected!\n");
    }

    /** Send a message to the given IP with the given contents. */
    void sendMessage(Message* msg, int fileDesc)
    {
        lock_.lock();
        //serialize message
        msg->serialize(s);

        int sendVal = send(fileDesc, s->getBuffer(), s->getNumBytesWritten(), 0);
        assert(sendVal >= 0);
        printf("Sent message contents: %s\n", msg);

        //clear serializer
        s->clear();
        
        lock_.unlock();
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    Message* receiveMessage(int fileDesc)
    {
        Message* tmp = new Message();
        //create buffer for message: will length be an issue?
        size_t buffLen = 2014;
        char* buffer = new char[buffLen];
        memset(buffer, 0, buffLen);

        int valread = read(fileDesc, buffer, buffLen);
        assert(valread >= 0);
        assert(buffer != nullptr);

        lock_.lock();
        //write to serializer and return deserialized result
        s->write(buffLen, buffer);
        tmp->deserialize(s);
        lock_.unlock();

        //receiver of message owns it?
        return tmp; 
    }

    /** Get file descriptor that this node listens on */
    int getFd()
    {
        return fd;
    }

    /** Get node id */
    size_t getNodeId()
    {
        return nodeId_;
    }

    /** Return address of this node */
    sockaddr_in getAddr()
    {
        return address;
    }

    /** Binds bock to address and corresponding port number.
     * Only needed by socket.*/
    void bindSocket_()
    {
        int val = bind(fd, (struct sockaddr *)&address,
                       sizeof(address));

        if (val < 0) {
            fprintf(stderr, "ERRNO: %d\n", errno);
        }
        assert(val >= 0);
        printf("Server binded to socket\n");
    }
};
