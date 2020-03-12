//lang:cpp

#include "../utils/object.h"
#pragma once

/** This class will hold information about a client that a server will use to communicate with it.
 * It will include the client's IP, port number, and file descriptor of the connected socket. 
 * authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu */
class ClientData : public Object
{
public:
    const char *ip_; //client's ip address
    int port_;       //port to reach client on
    int fd_;         //file descriptor for client's socket

    ClientData(const char *ip, int port, int sockFd)
    {
        ip_ = ip;
        port_ = port;
        fd_ = sockFd;
    }

    /** String representation of this data. For testing purposes */
    char* toString()
    {
        char* rep = new char[100];
        memset(rep, 0, 100);
        sprintf(rep, "%s:%d:%d,", ip_, port_, fd_);

        return rep;
    }

    /** Get Client's file descriptor */
    int getClientFd()
    {
        return fd_;
    }

    /** Get Client's IP */
    char* getIp()
    {
        return (char*)ip_;
    }
};
