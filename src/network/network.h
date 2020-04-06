#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "../utils/thread.h"
#include "message.h"
#include "msgkind.h"

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
    String* ipAddress_;

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

        ipAddress_ = new String(addr);
        address.sin_family = AF_INET;
        int inet = inet_pton(AF_INET, addr, &address.sin_addr);
        assert(inet > 0);
        address.sin_port = htons(8080);
        bindSocket_();
        listenForConnections();
    }

    ~Network()
    {
        delete s;
        delete dir;
        delete ipAddress_;
    }

    void server_init() {
        // bind a socket
        // - done in the constructor for now

        size_t num_nodes = 2;
        int* sockets = new int[num_nodes]; //sockets[i] is talking to node i
        // accept conns from nodes that want to register
        listenForConnections();
        // Build a Directory from the msgs each node sends
        for (int i = 1; i < num_nodes; i++) {
            //int tmp = acceptConnection();
            RegisterMsg* rMsg = dynamic_cast<RegisterMsg*>(receiveMsg()); //blocking
            assert(rMsg);
            handleRegisterMsg(rMsg);
            fprintf(stderr, "Server handled message #%d\n", i);
            //size_t node = rMsg->getSender();
            //sockets[node] = tmp;
            //dir->addIp(node, rMsg->getClient());
            //delete clientInfo; //depends on what this deletes (the actual objects stored in Dir?)
            //delete rMsg; //depends on what this deletes, clone data first?
        }
        fprintf(stderr, "Server got all the nodes\n");
        // When all nodes (expected number) have registered, send directory to everyone
        for (int i = 1; i < num_nodes; i++) {
            DirectoryMsg* dMsg = new DirectoryMsg(dir, 8080, nodeId_, i, 0);
            fprintf(stderr, "Server sending Directory Message to Node %d\n", i);
            sendMsg(dMsg);
        }

        // Close all the sockets
        for (int i = 1; i < num_nodes; i++) {
            int closeVal = close(sockets[i]);
            assert(closeVal >= 0);
            fprintf(stderr, "Server closed socket %d\n", i);
        }
        // ??Figure out when to send terminate msgs??
    }

    void client_init() {
        // bind a socket to server
        // - in constructor we bind to a fd
        size_t numNodes = 2;
        // send register msg to server
        String* servIp = new String(SERVER_IP);
        dir->addIp(0, servIp);
        RegisterMsg* rMsg = new RegisterMsg(ipAddress_, 8080, nodeId_, SERVER_NODE_NUM, 0);
        sendMsg(rMsg);
        delete servIp;
        fprintf(stderr, "Node %zu Registered\n", nodeId_);

        //handle directory msg
        for (int i = 1; i < numNodes; i++)
        {
            Message* m = receiveMsg();
            assert(m->getKind() == MsgKind::Dir);
            fprintf(stderr, "Node %zu received directory message\n", nodeId_);
            handleDirectoryMsg(dynamic_cast<DirectoryMsg*>(m));
        }
        printf("Client done initializing\n");
    }


    /** Allows node to wait for other nodes to make connection to it */
    void listenForConnections()
    {
        //second parameter doesn't seem like a huge deal
        int val = listen(fd, 32);
        assert(val >= 0);
        fprintf(stderr, "Node %zu listening for connections...\n", nodeId_);
    }

    /** Pulls first connection requests off queue of requests.
     * Will only be used by server */
    int acceptConnection()
    {
        int newSock = accept(fd, (struct sockaddr *)&address,
                      (socklen_t *)&addresslen);
                      
        assert(newSock >= 0);
        fprintf(stderr, "Node %zu accepted connection!\n", nodeId_);

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
        fprintf(stderr, "Node %zu called sendMsg\n", nodeId_);
        int targetFd = socket(AF_INET, SOCK_STREAM, 0);
        if (targetFd < 0)
        {
            fprintf(stderr, "SETSOCKOPT ERRNO: %d\n", errno);
            assert(false);
        }
        assert(targetFd >= 0);
        //hardcode options for now
        int rc = setsockopt(targetFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        //SO_REUSEADDR | SO_REUSEPORT
        //assert(rc >= 0);
        if (rc < 0)
        {
            fprintf(stderr, "SETSOCKOPT ERRNO: %d\n", errno);
            assert(false);
        }
        assert(rc >= 0);

        struct sockaddr_in targetAddr;
        targetAddr.sin_family = AF_INET;
        fprintf(stderr, "Node %zu About to get address for node %zu\n", nodeId_, msg->getTarget());
        String* targetIp = dir->getAddress(msg->getTarget());
        fprintf(stderr, "Node %zu Got address %s\n", nodeId_, targetIp->c_str());
        int inet = inet_pton(AF_INET, targetIp->c_str(), &targetAddr.sin_addr);
        assert(inet > 0);
        targetAddr.sin_port = htons(8080);
        fprintf(stderr, "Node %zu Attempting to bind socket: fd %d, port %d, ip %s\n", nodeId_, targetFd, targetAddr.sin_port,
        targetIp->c_str());
        // int val = bind(targetFd, (struct sockaddr *)&targetAddr, sizeof(targetAddr));

        // if (val < 0) {
        //     fprintf(stderr, "BIND ERRNO: %d\n", errno);
        //     assert(false);
        // }
        //fprintf(stderr, "Node %zu binded to other node %zu\n", nodeId_, msg->getTarget());


        // create conn to client
        int connection = connect(targetFd, (struct sockaddr *)&targetAddr, sizeof(targetAddr));
        if (connection < 0) {
            fprintf(stderr, "Node %zu Connection BIND ERRNO: %d\n", nodeId_, errno);
            assert(false);
        }
        assert(connection >= 0);
        
        fprintf(stderr, "Node %zu Connection established to node %zu\n", nodeId_, msg->getTarget());

        //serialize and send message
        msg->serialize(s);
        int sendVal = send(targetFd, s->getBuffer(), s->getNumBytesWritten(), 0);
        assert(sendVal >= 0);
        fprintf(stderr, "Node %zu Sent message to %zu\n", nodeId_, msg->getTarget());

        //clear serializer
        s->clear();
        
        close(targetFd);
        lock_.unlock();
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    Message* receiveMsg()
    {
        //lock_.lock();
        int tmpFd = acceptConnection();
        Message* tmp = nullptr;
        //create buffer for message: will length be an issue?
        size_t buffLen = 4096;//2048;
        char* buffer = new char[buffLen];
        memset(buffer, 0, buffLen);

        fprintf(stderr, "Node %zu trying to read\n", nodeId_);
        int valread = read(tmpFd, buffer, buffLen);
        while (valread < 0) {
            // fprintf(stderr, "ERRNO: %d\n", errno);
            // assert(false);
            //Better way to do this?
            valread = read(tmpFd, buffer, buffLen);
        }
        assert(valread >= 0);
        assert(buffer != nullptr);
        close(tmpFd);

        fprintf(stderr, "Node %zu received message\n", nodeId_);
        char* msgTypeBuff = new char[buffLen];
        memcpy(msgTypeBuff, buffer, buffLen);
        Serializer* tmpSer = new Serializer(buffLen, msgTypeBuff);
        MsgKind type = tmpSer->readMsgKind();
        switch (type) {
            case GetData: tmp = new GetDataMsg(); break;
            case Put: tmp = new PutMsg(); break;
            case ReplyData: tmp = new ReplyDataMsg(); break;
            case WaitAndGet: tmp = new WaitAndGetMsg(); break;
            case Register: tmp = new RegisterMsg(); break;
            case Dir: tmp = new DirectoryMsg(); break;
            default:
            {
                fprintf(stderr, "Unknown message type\n");
                break;
            }
        }
        assert(tmp);
        //delete tmpSer;
        // ?? delete[] msgTypeBuff;

        lock_.lock();
		Serializer* myS = new Serializer(buffLen, buffer);
        tmp->deserialize(myS);
		//delete myS;
        /*s->write(buffLen, buffer);
        tmp->deserialize(s);
        s->clear();*/
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
        //add new client to directory
        size_t tmpS = dir->size();
        dir->addIp(m->getSender(), m->getClient());
        assert(dir->size() == tmpS + 1);

        fprintf(stderr, "Current Directory size = %zu\n", dir->size());

        //send out directory message to all nodes
        SizeTWrapper** nodes = dir->getNodes();
        for (size_t i = 0; i < dir->size(); i++)
        {
            DirectoryMsg* dm = new DirectoryMsg(dir, 8080, nodeId_, nodes[i]->asSizeT(), 0);
            fprintf(stderr, "About to send directory message from %zu to %zu\n", nodeId_, nodes[i]->asSizeT());
            sendMsg(dm);
            fprintf(stderr, "Server sent DirectoryMessage to node %zu\n", nodes[i]->asSizeT());
            delete dm;
            //delete nodes[i];
        }
        //delete[] nodes;   
    }

    /** Handle directory message */
    void handleDirectoryMsg(DirectoryMsg* m)
    {
        fprintf(stderr, "Directory message received on node %zu\n", nodeId_);
        //current directories call new directory
        dir->mergeIn(m->getDirectory());
        fprintf(stderr, "Directory message on node %zu: directory merged in\n", nodeId_);
    }

    /** Binds bock to address and corresponding port number.
     * Only needed by socket.*/
    void bindSocket_()
    {  
        fprintf(stderr, "bNode %zu Attempting to bind socket: fd %d, port %d, ip %s\n", nodeId_, fd, address.sin_port,
        ipAddress_->c_str());
        int val = bind(fd, (struct sockaddr *)&address,
                       sizeof(address));

        if (val < 0) {
            fprintf(stderr, "ERRNO: %d\n", errno);
        }
        assert(val >= 0);
        fprintf(stderr, "Node %zu binded to socket(%d)\n", nodeId_, fd);
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
