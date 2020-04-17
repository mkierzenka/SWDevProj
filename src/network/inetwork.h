#pragma once

#include "../utils/object.h"
#include "message.h"

/**
 * Network interface for the Distributed KeyValue store.
 * Outlines what behavior a Network needs to support to be compatible with our system.
 *
 * @authors Marcin Kierzenka and Chase Broder
 */
class INetwork : public Object
{
public:
    /**
     * Send the given message through this network. This method steals ownership
     * of the message passed in.
     */
    virtual void sendMsg(Message* m) = 0;

    /**
     * Receives the next message over the network (blocking).
     */
    virtual Message* receiveMsg() = 0;

    /**
     * Initializes the server node for this distributed KV Store.
     * Each application running should only have a single server.
     */
    virtual void server_init() = 0;

    /**
     * Initializes the client node for this distributed KV Store.
     * An application may support multiple clients at once.
     */
    virtual void client_init() = 0;

    /**
     * Outlines how this network should react to a RegisterMsg.
     * Steals ownership of the message passed in.
     * ASSUME: Input was already checked to ensure it's a non-nullptr RegisterMsg
     */
    virtual void handleRegisterMsg(RegisterMsg* m) = 0;

    /**
     * Outlines how this network should react to a DirectoryMsg.
     * Steals ownership of the message passed in.
     * ASSUME: Input was already checked to ensure it's a non-nullptr DirectoryMsg
     */
    virtual void handleDirectoryMsg(DirectoryMsg* m) = 0;

    /**
     * Outlines how this network should react to a DoneMsg.
     * Steals ownership of the message passed in.
     * ASSUME: Input was already checked to ensure it's a non-nullptr DoneMsg
     */
    virtual void handleDoneMsg(DoneMsg* m) = 0;
};
