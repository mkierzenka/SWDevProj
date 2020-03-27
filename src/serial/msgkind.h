//lang::CwC

#pragma once


enum MsgKind {
    Ack, Nack, Put, ReplyData, Get, WaitAndGet, Status, Kill, Register, Directory, GetData
};