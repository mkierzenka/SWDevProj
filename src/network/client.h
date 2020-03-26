#pragma once

#include "../utils/object.h"
#include "network.h"
#include "../utils/array.h"
#include "clientdata.h"
#include "../utils/string.h"
#include "integer.h"
#include <poll.h>

#include <string.h>

/** This class represents a Client object. Its purpose is to
 * register itself with a server, and connect with any other clients
 * registered to that server. */
class Client : public Object
{
public:
    const char *clientIp_;
    const char *serverIp_;
    Network *network_;   //network to connect to server
    Network *listening_; //network to listen for client connections
    bool shouldSend_;    //for demo purposes, so one client knows to send to other

    Array *clients_; //connection info of neighboring clients
    Array *fdArr;

    Client(const char *selfIp, const char *servIp)
    {
        clientIp_ = selfIp;
        serverIp_ = servIp;
        network_ = new Network(servIp);
        printf("Server network established\n");
        listening_ = new Network(selfIp);
        listening_->bindSocket();
        printf("Binded listening socket\n");
        listening_->listenForClient(listening_->getServerFd());
        clients_ = new Array();
        fdArr = new Array();
        shouldSend_ = false;
    }

    ~Client()
    {
        delete network_;
        delete clients_;
        delete fdArr;
    }

    /** This method will register this client with the given server. If successful,
     * then it will receive a list of clients that are already connected to the
     * server. */
    void registerSelf()
    {
        network_->connectToServer();
        puts("Client registered!");

        char msg[100];
        strcpy(msg, "REGISTER ");
        strcat(msg, clientIp_);
        //send IP over
        network_->sendMessage(msg, network_->getServerFd());
    }

    /** Get message type and handle it accordingly */
    void handleMessage(int anFD)
    {
        char *msg = network_->receiveMessage(anFD);
        char *saveBuf;
        printf("Received message: %s\n", msg);

        //pull message type from message
        const char *msgType = strtok_r(msg, " ", &saveBuf);

        printf("Message type: %s\n", msgType);
        if (strcmp(msgType, "UPDATE") == 0)
        {
            updateClientConnections_(strtok_r(NULL, " ", &saveBuf));
        }
        else if (strcmp(msgType, "TEARDOWN") == 0)
        {
            closeConnection_();
        }

        delete msg;
    }

    int acceptNewClient()
    {
        int newSock = listening_->acceptConnection(listening_->getServerFd());
        fdArr->add(new Integer(newSock));
        puts("NEW CLIENT CONNECTION TO OTHER CLIENT\n");

        return newSock;
    }

    struct pollfd *getFDs()
    {
        struct pollfd *ret = new struct pollfd[2 + fdArr->length()];
        // First fd is the server connection
        ret[0].fd = network_->getServerFd();
        ret[0].events = POLLIN | POLLOUT;

        // Second fd is where this client listens
        ret[1].fd = listening_->getServerFd();
        ret[1].events = POLLIN;

        // Rest are the client connections we have
        for (int i = 0; i < fdArr->length(); i++)
        {
            Integer *tmp = dynamic_cast<Integer *>(fdArr->get(i));
            ret[2 + i].fd = tmp->val_;
            ret[2 + i].events = POLLIN | POLLOUT;
        }

        return ret;
    }

    /** Read in list of client connections. If there is a new connection, then add it to ClientData */
    void updateClientConnections_(char *msg)
    {
        //this would happen upon first client connecting (no other clients to connect to)
        if (msg == nullptr)
        {
            return;
        }

        //buffer for tokenizing message
        char *saveBuf;

        //clear client data
        clients_->clear();

        //rebuild client data list
        char *ip = strtok_r(msg, ",", &saveBuf);
        int i = 0;
        while (ip != nullptr)
        {
            parseClientData_(ip);
            connectAndSend_(ip);

            ip = strtok_r(NULL, ",", &saveBuf);
            i++;
        }
    }

    /** Takes client data in string form, converts it into object, and adds it to list */
    void parseClientData_(char *c)
    {
        //buffer for tokenizing
        char *saveBuf;
        char *ip = strtok_r(c, ":", &saveBuf);
        printf("IP: %s\n", ip);
        char *portStr = strtok_r(NULL, ":", &saveBuf);
        int port = atoi(portStr);
        printf("PORT: %d\n", port);
        char *fdStr = strtok_r(NULL, ",", &saveBuf);
        int fd = atoi(fdStr);
        printf("FD: %d\n", fd);

        addClientData_(ip, port, fd);
    }

    /** Build a client data object from the parameters and add it to the client's list */
    void addClientData_(char *ip, int port, int fd)
    {
        ClientData *c = new ClientData(ip, port, fd);
        clients_->add(c);
    }

    void connectAndSend_(char *ip)
    {
        printf("Attempting to connect to client %s\n", ip);
        Network *n = new Network(ip);
        n->connectToServer();
        printf("Conneted to %s\n", ip);
        n->sendMessage("HIIII", n->getServerFd());
    }

    //close listening socket of client
    void closeConnection_()
    {
        printf("Received teardown message\n");
        //close listening fd and exit
        int closeVal = close(listening_->getServerFd());
        assert(closeVal >= 0);
        printf("Closed socket %d\n", listening_->getServerFd());
        exit(1);
    }
};