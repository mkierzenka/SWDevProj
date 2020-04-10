#pragma once

#include "../utils/string.h"
#include "../utils/object.h"
#include "network.h"
#include "../utils/array.h"
#include "clientdata.h"

#include <string.h>
#include <poll.h>

/** This class represents a server. It will listen
 * to requests from clients. Upon a client registering
 * with the server, it will send an update message to the rest of
 * its clients. */
class Server : public Object
{
public:
    Network *network_; //node layer of this server
    //const char *ip_; //ip of this server
    Array *clients_;  //object representations of client information subscribed to server

    Server(const char *ip, size_t nodeId)
    {
        network_ = new Network(ip, nodeId);
        network_->listenForConnections();
        clients_ = new Array();
    }

    ~Server()
    {
        delete network_;
        delete clients_;
    }

    /** Accept a new node connection */
    void acceptConnection()
    {
        //accept connection
        int stagingSock_ = network_->acceptConnection();
        printf("New node accepted!\n");
    }

    // /** Get message type and handle it accordingly */
    // void handleMessage()
    // {
    //     char *msg = network_->receiveMessage(stagingSock_);
    //     printf("Received message: %s\n", msg);

    //     //pull message type from message
    //     const char *msgType = strtok(msg, " ");
    //     printf("Message type: %s\n", msgType);
    //     assert(msgType != nullptr);
    //     if (strcmp(msgType, "REGISTER") == 0)
    //     {
    //         const char *ip = strtok(NULL, " ");
    //         assert(ip != nullptr);
    //         handleIp_(ip);
    //         sendUpdates_();
    //     }

    //     delete msg;
    // }

    /** Close the server and connected clients */
    // void initiateTeardown()
    // {
    //     //tell all clients they need to close their sockets
    //     for (int i = 0; i < clients_->length(); i++)
    //     {
    //         ClientData* cdata = dynamic_cast<ClientData *>(clients_->get(i));
    //         network_->sendMessage("TEARDOWN", cdata->getClientFd());
    //     }

    //     int closeVal = close(fd_);
    //     assert(closeVal >= 0);
    //     printf("Closed socket %d\n", fd_);
    //     exit(1);
    // }

    // /** Return poll struct for listening */
    // struct pollfd *getPollObj()
    // {
    //     struct pollfd *fdStruct = new struct pollfd[1];
    //     fdStruct[0].fd = fd_;
    //     fdStruct[0].events = POLLIN;

    //     return fdStruct;
    // }

    /** Add ip to list and broadcast this list to all connected clients */
    void handleIp_(const char *ip)
    {
        //build client data object
        ///TODO: pass port into server
        ClientData *c = new ClientData(strdup(ip), 8080, stagingSock_);

        clients_->add(c);
        printf("New client added to server's registered list: %s", c->toString());
        //c->print();
        printf("\nList now contains %zu clients\n", clients_->length());
    }

    String *genMsgForIdx_(size_t idx)
    {
        ClientData *cdata = nullptr;
        StrBuff buff;
        buff.c("UPDATE ");
        char *tmp;
        for (size_t i = 0; i < idx; i++)
        {
            cdata = dynamic_cast<ClientData *>(clients_->get(i));
            tmp = cdata->toString();
            buff.c(tmp);
            delete tmp;
        }

        //skip own client: doesn't need to connect to itself
         for (size_t i = idx + 1; i < clients_->length(); i++)
         {
             cdata = dynamic_cast<ClientData *>(clients_->get(i));
             tmp = cdata->toString();
             buff.c(tmp);
             delete tmp;
         }
        return buff.get();
    }

    void sendUpdates_()
    {
        char *msg = nullptr;

        for (size_t i = 0; i < clients_->length(); i++)
        {
            ClientData* clientInfo = dynamic_cast<ClientData*>(clients_->get(i));
            msg = genMsgForIdx_(i)->c_str();
            printf("msg: [%s]\n", msg);
            if (msg && strlen(msg) > 0)
            {
                printf("sending update to %s\n", clientInfo->getIp());
                network_->sendMsg(msg);
            }
        }
    }
};
