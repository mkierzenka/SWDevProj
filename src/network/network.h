#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "../../utils/thread.h"
#include "message.h"
#include "msgkind.h"
#include "sockaddrwrapper.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <assert.h>

const char* SERVER_IP = "127.0.0.1"; //hardcoded server IP (for now)
const size_t SERVER_NODE_NUM = 0;

/** This class wraps the basic functionality of the POSIX libraries.
 * It is used to make connections between clients and the server. */
class Network : public Object
{

public:
    Serializer* s; //used for sending and receiving message
    Directory* dir; //directory of other nodes on program
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
        dir = new Directory();
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
        delete dir;
    }

    void server_init() {
        // bind a socket
        // - done in the constructor for now

        size_t num_nodes = 3;
        int* sockets = new int[num_nodes]; //sockets[i] is talking to node i
        // accept conns from nodes that want to register
        listenForConnections();
        // Build a Directory from the msgs each node sends
        for (int i = 1; i < num_nodes; i++) {
            int tmp = acceptConnection();
            RegisterMsg* rMsg = dynamic_cast<RegisterMsg*>(receiveMsg()); //blocking
            assert(rMsg);
            size_t node = rMsg->getSender();
            sockets[node] = tmp;
            SockAddrWrapper* clientInfo = new SockAddrWrapper(rMsg->getClientInfo());
            dir->addClientInfo(node, clientInfo);
            //delete clientInfo; //depends on what this deletes (the actual objects stored in Dir?)
            //delete rMsg; //depends on what this deletes, clone data first?
        }
        // When all nodes (expected number) have registered, send directory to everyone
        for (int i = 1; i < num_nodes; i++) {
            DirectoryMsg* dMsg = new DirectoryMsg(dir, 8080, nodeId_, i, 0);
            sendMsg(dMsg);
        }

        // Close all the sockets
        for (int i = 1; i < num_nodes; i++) {
            int closeVal = close(sockets[i]);
            assert(closeVal >= 0);
            printf("Closed socket %d\n", i);
        }
        // ??Figure out when to send terminate msgs??
    }

    void client_init() {
        // bind a socket to server
        // - in constructor we bind to a fd
        
        // send register msg to server

        RegisterMsg* rMsg = new RegisterMsg(address, 8080, nodeId_, SERVER_NODE_NUM, 0);
        sendMsg(rMsg);
        // ReceiverThread will handle the DirectoryMsg response
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
    void sendMsg(Message* msg)
    {
        lock_.lock();
        //serialize message


        int targetFd = socket(AF_INET, SOCK_STREAM, 0);
        assert(targetFd > 0);
        //hardcode options for now
        int rc = setsockopt(targetFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        assert(rc >= 0);

        struct sockaddr_in targetAddr;
        targetAddr.sin_family = AF_INET;
        char* targetIp = dir->getAddress(msg->getTarget());
        int inet = inet_pton(AF_INET, targetIp, &targetAddr.sin_addr);
        assert(inet > 0);
        targetAddr.sin_port = htons(8080);
        int val = bind(targetFd, (struct sockaddr *)&targetAddr, sizeof(targetAddr));

        if (val < 0) {
            fprintf(stderr, "ERRNO: %d\n", errno);
            assert(false)
        }
        printf("Client binded to other client\n");


        // create conn to client
        int connection = connect(targetFd, (struct sockaddr *)&targetAddr, sizeof(targetAddr));
        assert(connection >= 0);
        
        //serialize and send message
        msg->serialize(s);
        int sendVal = send(targetFd, s->getBuffer(), s->getNumBytesWritten(), 0);
        assert(sendVal >= 0);
        printf("Sent message contents: %s\n", msg);

        //clear serializer
        s->clear();
        
        lock_.unlock();
        close(targetFd);
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    Message* receiveMsg()
    {
        Message* tmp = nullptr;
        //create buffer for message: will length be an issue?
        size_t buffLen = 2014;
        char* buffer = new char[buffLen];
        memset(buffer, 0, buffLen);

        int valread = read(fd, buffer, buffLen);
        assert(valread >= 0);
        assert(buffer != nullptr);

        char* msgTypeBuff = new char[buffLen];
        memcpy(msgTypeBuff, buffer, buffLen);
        Serializer* tmpSer = new Serializer(msgTypeBuff, buffLen);
        MsgKind type = tmpSer->readMsgKind();
        switch (type) {
            case GetData: tmp = new GetDataMsg(); break;
            case Put: tmp = new PutMsg(); break;
            case ReplyData: tmp = new ReplyDataMsg(); break;
            case WaitAndGet: tmp = new WaitAndGetMsg(); break;
            case Register: tmp = new RegisterMsg(); break;
            case Dir: tmp = new DirMsg(); break;
            default:
            {
                fprintf(stderr, "Unknown message type\n");
                break;
            }
        }
        assert(tmp);
        delete tmpSer;
        // ?? delete[] msgTypeBuff;

        lock_.lock();
        s->write(buffLen, buffer);
        tmp->deserialize(s);
        s->clear();
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

    /** Handle register message received on this node */
    void handleRegisterMsg(RegisterMsg* m)
    {
        //add new client to directory and broadcast out to other nodes
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

    sockaddr_in charToAddr_(char* addr)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        int inet = inet_pton(AF_INET, addr, &add.sin_addr);
        assert(inet > 0);
        add.sin_port = htons(8080);

        return add;
    }
};
