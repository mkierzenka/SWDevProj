#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "../utils/thread.h"
#include "../utils/args.h"
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

/** This class wraps the basic functionality of the POSIX libraries.
 * It is used to make connections between clients and the server.
 *
 * @authors Marcin Kierzenka and Chase Broder
 */
class Network : public INetwork
{

public:
    Serializer* mySer_;        //owned, used for sending and receiving message
    Directory* dir_;           //owned, directory of other nodes on program
    Lock lock_;                //lock for reading from and writing to serializer
    int fd;                    //descriptor to send message to this node
    struct sockaddr_in myAddr;      //address struct for me
    int myAddrLen = sizeof(myAddr);
    bool* isNodeDone_; //owned, arr to track which nodes have completed their work


    Network():INetwork()
    {
        mySer_ = new Serializer();
        dir_ = new Directory();
        fd = setupNewSocket_();
		myAddr = charToAddr_(args.ip, args.port);
        bindSocket_();
        listenForConnections_();

        isNodeDone_ = new bool[args.numNodes];
        for (size_t i = 0; i < args.numNodes; i++) {
            isNodeDone_[i] = false;
        }
        //add the server info to this network's directory (even if we are server)
        dir_->addInfo(args.serverIndex, args.serverIp, args.serverPort);
    }

    ~Network()
    {
        delete mySer_;
        delete dir_;
        delete[] isNodeDone_;
        close(fd);
    }

    void server_init() override {
        // Build a Directory from the msgs each node sends
        for (int i = 1; i < args.numNodes; i++) {
            RegisterMsg* rMsg = dynamic_cast<RegisterMsg*>(receiveMsg()); //blocking
            assert(rMsg);
            handleRegisterMsg(rMsg);
        }
        fprintf(stderr, "Server got all the nodes\n");
        // When all nodes (expected number) have registered, send directory to everyone
        for (int i = 0; i < args.numNodes; i++) {
            if (i == args.serverIndex) continue;
            DirectoryMsg* dMsg = new DirectoryMsg(dir_, args.index, i);
            fprintf(stderr, "Server sending Directory Message to Node %d\n", i);
            sendMsg(dMsg);
        }
    }

    void client_init() override {
        RegisterMsg* rMsg = new RegisterMsg(args.ip, args.serverPort, args.index, args.serverIndex);
        sendMsg(rMsg);
        fprintf(stderr, "Node %zu Registered\n", args.index);
		Message* m = receiveMsg();
		assert(m->getKind() == MsgKind::Dir);
		handleDirectoryMsg(dynamic_cast<DirectoryMsg*>(m));
    }

    // Implementation Notes: Assume the data sent across the network is serialized
    // message kind followed by the complete serialized message
    void sendMsg(Message* msg) override
    {
        int targetFd = setupNewSocket_();
        NodeInfo* targetInfo = dir_->getInfo(msg->getTarget());

        // create conn to client
        connectToNode_(targetFd, targetInfo->getAddr());
        //serialize and send message
        lock_.lock();
        mySer_->clear();
        mySer_->write(msg->getKind());
        msg->serialize(mySer_);
        int sendVal = send(targetFd, mySer_->getBuffer(), mySer_->getNumBytesWritten(), 0);
        crashIfError_("send error", sendVal >= 0);
        mySer_->clear();
        lock_.unlock();
        close(targetFd);
        delete msg;
    }

    // Implementation Notes: Assume the data sent across the network is serialized
    // message kind followed by the complete serialized message
    Message* receiveMsg() override
    {
        int tmpFd = acceptConnection_();
        String* buffStr = readStrFromNet_(tmpFd); //Complete set of bytes received on net
        close(tmpFd);
        lock_.lock();
        mySer_->write(buffStr->size(), buffStr->c_str());
        Message* tmp = nullptr;
        MsgKind type = mySer_->readMsgKind(); //assume msgkind is serialized before msg
        switch (type) {
            case GetData: tmp = new GetDataMsg(); break;
            case Put: tmp = new PutMsg(); break;
            case ReplyData: tmp = new ReplyDataMsg(); break;
            case WaitAndGet: tmp = new WaitAndGetMsg(); break;
            case Register: tmp = new RegisterMsg(); break;
            case Dir: tmp = new DirectoryMsg(); break;
            case Done: tmp = new DoneMsg(); break;
            case Teardown: tmp = new TeardownMsg(); break;
            default:
            {
                fprintf(stderr, "Unknown message type\n");
                break;
            }
        }
        assert(tmp);
        tmp->deserialize(mySer_);
        delete buffStr;
        lock_.unlock();
        return tmp;
    }

    void handleRegisterMsg(RegisterMsg* m) override
    {
        //add new client to directory
        size_t sizeBefore = dir_->size();
        dir_->addInfo(m->getSender(), m->getInfo());
        assert(dir_->size() == sizeBefore + 1);
        delete m;
    }

    void handleDirectoryMsg(DirectoryMsg* m) override
    {
        //current directories call new directory
        dir_->mergeIn(m->getDirectory());
        delete m;
    }

    /** For this (real) Network, only the server calls this */
    void handleDoneMsg(DoneMsg* m) override
    {
        assert(m && args.index == args.serverIndex);
        isNodeDone_[m->getSender()] = true;
        delete m;
        for (size_t i = 0; i < args.numNodes; i++) {
            if (!isNodeDone_[i]) {
                return; // Not all the nodes are done yet
            }
        }
        
        sendTeardownMsgs_();
    }

   /** Creates a new socket with our standard options. Crash if error. */
    int setupNewSocket_() {
        int newSock = socket(AF_INET, SOCK_STREAM, 0);
        crashIfError_("socket error", newSock >= 0);
        int opt = 1;
        int rc = setsockopt(newSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        crashIfError_("setsockopt SO_REUSEADDR error", rc == 0);
        rc = setsockopt(newSock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        crashIfError_("setsockopt SO_REUSEPORT error", rc == 0);
        return newSock;
    }

    /** Binds sock to address and corresponding port number. */
    void bindSocket_()
    {  
        int val = bind(fd, (struct sockaddr *)&myAddr, (socklen_t)myAddrLen);
        crashIfError_("bind error", val == 0);
        fprintf(stderr, "Node %zu binded to socket(%d)\n", args.index, fd);
    }

    /** Constructs a new sockaddr_in from given ip and port */
    struct sockaddr_in charToAddr_(char* addr, size_t port)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        int inet = inet_pton(AF_INET, addr, &add.sin_addr);
        crashIfError_("inet pton error", inet == 1);
        add.sin_port = htons(port);
        return add;
    }

    /** Allows node to wait for other nodes to make connection to it */
    void listenForConnections_()
    {
        int val = listen(fd, 32);
        crashIfError_("listen error", val >= 0);
    }

    /** Pulls first connection requests off queue of requests. */
    int acceptConnection_()
    {
        int newSock = accept(fd, (struct sockaddr *)&myAddr, (socklen_t *)&myAddrLen);
        crashIfError_("accept error", newSock >= 0);
        return newSock;
    }

    /** Attempt to connect to socket with given fd */
    void connectToNode_(int sockFd, struct sockaddr_in addr)
    {
        int conn = connect(sockFd, (struct sockaddr *)&addr, sizeof(addr));
        crashIfError_("connect error", conn >= 0);
    }

    /**
	 * Handles details or reading arbitrary length message from a file descriptor.
	 * Reads in data until none left (receives EOF).
	 * Returns String* with the serialized message data.
	 * ASSUME: sendersFd came from acceptConnection() already
	 */
    String* readStrFromNet_(size_t sendersFd) {
        size_t buffLen = 8192;
        char* buffer = new char[buffLen];
        memset(buffer, 0, buffLen);
        //fprintf(stderr, "Node %zu trying to read\n", args.index);
        int valread = read(sendersFd, buffer, buffLen);
        while (valread < 0) {
            valread = read(sendersFd, buffer, buffLen);
        }
        //received message: now read until no more bytes to read
        StrBuff* buff = new StrBuff();
        while (valread > 0)
        {
            buff->c(buffer, valread);
            memset(buffer, 0, buffLen);
            valread = read(sendersFd, buffer, buffLen);
        }
        crashIfError_("receiveMsg read error", valread >= 0);
        delete[] buffer;
        String* buffStr = buff->get();
        delete buff;
        return buffStr;
    }

    /**
     * A helper to send teardown messages to all the nodes (including its own ReceiverThread)
     * Only the Server should call this method
     */
    void sendTeardownMsgs_() {
        printf("Server starting teardown\n");
        for (size_t i = 0; i < args.numNodes; i++) {
            sendMsg(new TeardownMsg(args.index, i));
        }
    }

	/** If condition fails, print perror with message and assert false */
    void crashIfError_(const char* msg, bool success) {
        if(success) return;
        perror(msg);
        assert(false);
    }
};
