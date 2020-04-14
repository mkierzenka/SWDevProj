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
 * It is used to make connections between clients and the server. */
class Network : public INetwork
{

public:
    Serializer* mySer_;        //used for sending and receiving message
    Directory* dir_;           //directory of other nodes on program
    Lock lock_;                //lock for reading from and writing to serializer
    int fd;                    //descriptor to send message to this node
    struct sockaddr_in myAddr;      //address struct for me
    int myAddrLen = sizeof(myAddr);
    bool* isNodeDone_; //array to keep track of which nodes have completed their work

    /**
     * Network constructor. Will take string IP of the client/server
     * that it was initialized by.
	 */
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
        String* servIp = new String(args.serverIp);
        //add the server info to this network's directory (even if we are server)
        dir_->addIp(args.serverIndex, servIp);
        delete servIp;
    }

    ~Network()
    {
        delete mySer_;
        delete dir_;
        delete[] isNodeDone_;
        close(fd);
    }

    void server_init() {
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
            DirectoryMsg* dMsg = new DirectoryMsg(dir_, 8080, args.index, i, args.index);
            fprintf(stderr, "Server sending Directory Message to Node %d\n", i);
            sendMsg(dMsg);
        }
    }

    void client_init() {
        RegisterMsg* rMsg = new RegisterMsg(args.ip, args.serverPort, args.index, args.serverIndex, 0);
        sendMsg(rMsg);
        fprintf(stderr, "Node %zu Registered\n", args.index);
		Message* m = receiveMsg();
		assert(m->getKind() == MsgKind::Dir);
		handleDirectoryMsg(dynamic_cast<DirectoryMsg*>(m));
        fprintf(stderr, "Node %zu done initializing\n", args.index);
    }

    /** Send a message to the given IP with the given contents. This method deletes the message
     * passed in once it's done sending.
    */
    void sendMsg(Message* msg)
    {
        fprintf(stderr, "Node %zu called sendMsg\n", args.index);
        int targetFd = setupNewSocket_();
        String* targetIp = dir_->getAddress(msg->getTarget());
        struct sockaddr_in targetAddr = charToAddr_(targetIp->c_str(), 8080);

        // create conn to client
        connectToNode_(targetFd, targetAddr);
        fprintf(stderr, "Node %zu Connection established to node %zu\n", args.index, msg->getTarget());

        //serialize and send message
        lock_.lock();
        mySer_->clear();
        msg->serialize(mySer_);
        int sendVal = send(targetFd, mySer_->getBuffer(), mySer_->getNumBytesWritten(), 0);
        crashIfError_("send error", sendVal >= 0);
        fprintf(stderr, "Node %zu Sent message to %zu\n", args.index, msg->getTarget());
        mySer_->clear();
        lock_.unlock();
        close(targetFd);
        delete msg;
    }

    /** Attempt to receive a message and return result.
	 * Caller is responsible for deleting return val.
	 */
    Message* receiveMsg()
    {
        int tmpFd = acceptConnection_();
        //create dynamically-sized buffer
        String* buffStr = readStrFromNet_(tmpFd);
        close(tmpFd);
        fprintf(stderr, "Node %zu received message\n", args.index);
        String* typeBuffStr = buffStr->clone(); //just for figuring out msg type
        lock_.lock();
        mySer_->write(typeBuffStr->size(), typeBuffStr->c_str());
        delete typeBuffStr;
        // WARNING: Tightly coupled to how we serialize messages!!
        Message* tmp = nullptr;
        MsgKind type = mySer_->readMsgKind();
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
        mySer_->clear();
        mySer_->write(buffStr->size(), buffStr->c_str());
        tmp->deserialize(mySer_);
        delete buffStr;
        lock_.unlock();
        return tmp;
    }

    /** Handle register message received on this node */
    void handleRegisterMsg(RegisterMsg* m)
    {
        //add new client to directory
        size_t tmpS = dir_->size();
        dir_->addIp(m->getSender(), m->getClient());
        assert(dir_->size() == tmpS + 1);
        fprintf(stderr, "Current Directory size = %zu\n", dir_->size());
    }

    /** Handle directory message */
    void handleDirectoryMsg(DirectoryMsg* m)
    {
        //current directories call new directory
        dir_->mergeIn(m->getDirectory());
        fprintf(stderr, "Node %zu: directory merged in\n", args.index);
    }

    /** Handle done message. Only for the server */
    void handleDoneMsg(DoneMsg* m)
    {
        assert(m && args.index == args.serverIndex);
        isNodeDone_[m->getSender()] = true;
        for (size_t i = 0; i < args.numNodes; i++) {
            if (!isNodeDone_[i]) {
                return; // Not all the nodes are done yet
            }
        }
        fprintf(stderr, "Server heard that all nodes are done\n");
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
        fprintf(stderr, "Node %zu listening for connections...\n", args.index);
    }

    /** Pulls first connection requests off queue of requests. */
    int acceptConnection_()
    {
        int newSock = accept(fd, (struct sockaddr *)&myAddr, (socklen_t *)&myAddrLen);
        crashIfError_("accept error", newSock >= 0);
        fprintf(stderr, "Node %zu accepted connection!\n", args.index);
        return newSock;
    }

    /** Attempt to connect to socket with given fd */
    void connectToNode_(int sockFd, struct sockaddr_in addr)
    {
        int conn = connect(sockFd, (struct sockaddr *)&addr, sizeof(addr));
        crashIfError_("accept error", conn >= 0);
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
        for (size_t i = 0; i < args.numNodes; i++) {
            sendMsg(new TeardownMsg(args.index, i, 0));
        }
    }

	/** If condition fails, print perror with message and assert false */
    void crashIfError_(const char* msg, bool success) {
        if(success) return;
        perror(msg);
        assert(false);
    }
};
