//lang::CwC

#pragma once


enum MsgKind {
    Ack, GetData, Put, ReplyData, WaitAndGet, Status, Register, Dir, Done, Teardown
};