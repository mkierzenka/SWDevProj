#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "../utils/thread.h"
#include "message.h"
#include "msgkind.h"
#include "inetwork.h"

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
class Network : public INetwork
{

public:
    Serializer* s;        //used for sending and receiving message
    Directory* dir;       //directory of other nodes on program
    Lock lock_;           //lock for reading from and writing to serializer
    int fd;               //descriptor to send message to this node
    size_t nodeId_;       //id of this node
    int opt = 1;
    struct sockaddr_in address;
    int addresslen = sizeof(address);
    int PORT = 8080;
    String* ipAddress_;

    /**
     * Network constructor. Will take string IP of the client/server
     * that it was initialized by.
	 */
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
        listenForConnections();
        // Build a Directory from the msgs each node sends
        for (int i = 1; i < NUM_NODES; i++) {
            RegisterMsg* rMsg = dynamic_cast<RegisterMsg*>(receiveMsg()); //blocking
            assert(rMsg);
            handleRegisterMsg(rMsg);
        }
        fprintf(stderr, "Server got all the nodes\n");
        // When all nodes (expected number) have registered, send directory to everyone
        for (int i = 1; i < NUM_NODES; i++) {
            DirectoryMsg* dMsg = new DirectoryMsg(dir, 8080, nodeId_, i, 0);
            fprintf(stderr, "Server sending Directory Message to Node %d\n", i);
            sendMsg(dMsg);
        }

        // ??Figure out when to send terminate msgs??
    }

    void client_init() {
        String* servIp = new String(SERVER_IP);
        dir->addIp(0, servIp);
        RegisterMsg* rMsg = new RegisterMsg(ipAddress_, 8080, nodeId_, SERVER_NODE_NUM, 0);
        sendMsg(rMsg);
        delete servIp;
        fprintf(stderr, "Node %zu Registered\n", nodeId_);

		Message* m = receiveMsg();
		assert(m->getKind() == MsgKind::Dir);
		handleDirectoryMsg(dynamic_cast<DirectoryMsg*>(m));
        fprintf(stderr, "Node %zu done initializing\n", nodeId_);
    }

    /** Allows node to wait for other nodes to make connection to it */
    void listenForConnections()
    {
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
        int connection = connect(fileDesc, (struct sockaddr *)&address,
                       sizeof(address));
        assert(connection >= 0);
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
        int rc = setsockopt(targetFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        if (rc < 0)
        {
            fprintf(stderr, "SETSOCKOPT ERRNO: %d\n", errno);
            assert(false);
        }
        assert(rc >= 0);

        struct sockaddr_in targetAddr;
        targetAddr.sin_family = AF_INET;
        String* targetIp = dir->getAddress(msg->getTarget());
        int inet = inet_pton(AF_INET, targetIp->c_str(), &targetAddr.sin_addr);
        assert(inet > 0);
        targetAddr.sin_port = htons(8080);

        // create conn to client
		int connection = connect(targetFd, (struct sockaddr *)&targetAddr, sizeof(targetAddr));
        if (connection < 0) {
            fprintf(stderr, "Node %zu Connection BIND ERRNO: %d\n", nodeId_, errno);
            assert(false);
        }
        assert(connection >= 0);
        fprintf(stderr, "Node %zu Connection established to node %zu\n", nodeId_, msg->getTarget());

        //serialize and send message
		Serializer* myS = new Serializer();
        msg->serialize(myS);
        int sendVal = send(targetFd, myS->getBuffer(), myS->getNumBytesWritten(), 0);
        assert(sendVal >= 0);
        fprintf(stderr, "Node %zu Sent message to %zu\n", nodeId_, msg->getTarget());
		delete myS;
        
        close(targetFd);
        lock_.unlock();
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    Message* receiveMsg()
    {
        int tmpFd = acceptConnection();
        Message* tmp = nullptr;
        //create dynamically-sized buffer
        StrBuff* buff = new StrBuff();
        size_t buffLen = 8192;
        char* buffer = new char[buffLen];
        memset(buffer, 0, buffLen);

        fprintf(stderr, "Node %zu trying to read\n", nodeId_);
        int valread = read(tmpFd, buffer, buffLen);
        while (valread < 0) {
            //Better way to do this?
            valread = read(tmpFd, buffer, buffLen);
        }

        //received message: now read until no more bytes to read
        while (valread > 0)
        {
            buff->c(buffer, valread);
            memset(buffer, 0, buffLen);
            valread = read(tmpFd, buffer, buffLen);
        }

        assert(valread == 0);
        assert(buffer != nullptr);
        close(tmpFd);

        fprintf(stderr, "Node %zu received message\n", nodeId_);
        char* msgTypeBuff = new char[buffLen];
        memcpy(msgTypeBuff, buffer, buffLen);
        String* buffStr = buff->get();
        Serializer* tmpSer = new Serializer(buff->size(), buffStr->c_str());
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
        Serializer* myS = new Serializer(buff->size(), buffStr->c_str());
        tmp->deserialize(myS);
        delete buffStr;
        delete[] buffer;
        delete buff;
        lock_.unlock();

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
        /*SizeTWrapper** nodes = dir->getNodes();
        for (size_t i = 0; i < dir->size(); i++)
        {
            DirectoryMsg* dm = new DirectoryMsg(dir, 8080, nodeId_, nodes[i]->asSizeT(), 0);
            fprintf(stderr, "About to send directory message from %zu to %zu\n", nodeId_, nodes[i]->asSizeT());
            sendMsg(dm);
            fprintf(stderr, "Server sent DirectoryMessage to node %zu\n", nodes[i]->asSizeT());
            delete dm;
            //delete nodes[i];
        }*/
        //delete[] nodes;   
    }

    /** Handle directory message */
    void handleDirectoryMsg(DirectoryMsg* m)
    {
        //current directories call new directory
        dir->mergeIn(m->getDirectory());
        fprintf(stderr, "Node %zu: directory merged in\n", nodeId_);
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
