#pragma once
#include "object.h"
#include <stdio.h>

/**
 * A class to parse and hold all of the command line arguments. This object will end up being exposed to
 * the entire program.
 * IMPORTANT: Need to call parse() to actually read in the commandline args. Otherwise it will be all defaults
 * 
 * @authors: kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */
class Arguments : public Object {
    public:
    char* appName;
    size_t numNodes;
    size_t index;
    char* ip;
    size_t port;
    char* serverIp;
    size_t serverPort;
    size_t serverIndex;
    bool usePseudoNet;
    size_t blockSize;

    Arguments() {
        appName = nullptr;//"trivial";
        numNodes = 1;
        index = 0;
        ip = nullptr;//"127.0.0.1";
        port = 8080;
        serverIp = nullptr;//"127.0.0.1";
        serverPort = 8080;
        serverIndex = 0;
        usePseudoNet = false;
        blockSize = 1024;
    }

    ~Arguments() {}

    void parse(int argc, char** argv) {
        for (int i = 1; i < argc; i++)
        {
            char* arg = argv[i];
            if (strcmp(arg, "-pseudo") == 0) {
                usePseudoNet = true;
                continue;
            }
            if (i == (argc - 1)) {
                usageError_(arg);
                exit(-1);
            }
            if (strcmp(arg, "-app") == 0) {
                appName = argv[++i];
            } else if (strcmp(arg, "-num_nodes") == 0) {
                numNodes = tryParseSizeT_(argv[++i]);
            } else if (strcmp(arg, "-i") == 0) {
                index = tryParseSizeT_(argv[++i]);
            } else if (strcmp(arg, "-ip") == 0) {
                ip = argv[++i];
            } else if (strcmp(arg, "-port") == 0) {
                port = tryParseSizeT_(argv[++i]);
            } else if (strcmp(arg, "-serverIdx") == 0) {
                serverIndex = tryParseSizeT_(argv[++i]);
            } else if (strcmp(arg, "-serverIp") == 0) {
                serverIp = argv[++i];
            } else if (strcmp(arg, "-serverPort") == 0) {
                serverPort = tryParseSizeT_(argv[++i]);
            } else if (strcmp(arg, "-blockSize") == 0) {
                blockSize = tryParseSizeT_(argv[++i]);
            } 
            else {
                usageError_(arg);
            }
        }
    }

    /** Print the arguments. For testing/debugging purposes */
    void print()
    {
        printf("App name: %s\n", appName);
        printf("Num nodes: %zu\n", numNodes);
        printf("Node index: %zu\n", index);
        printf("IP: %s\n", ip);
        printf("Port: %zu\n", port);
        printf("Server IP: %s\n", serverIp);
        printf("Server port: %zu\n", serverPort);
        printf("Server index: %zu\n", serverIndex);
        printf("Use pseudo network: %d\n", usePseudoNet);
    }


    /** Try to get a numerical value from a string. Error if invalid */
    size_t tryParseSizeT_(char* arg)
    {
        size_t val = 0;
        if (sscanf(arg, "%zu", &val) != 1)
        {
            fprintf(stderr, "Could not parse %s into a size_t\n", arg);
            exit(1);
        }
        //scan was successful: return the value
        return val;
    }

    void usageError_(char* arg) {
        fprintf(stderr, "Usage Error for argument (%s). Expected: ./eau2 -app (appname) -num_nodes N -i (mynodenum) -pseudo -ip (myipaddr) -port N -serverIp (addr) -serverPort N\n", arg);
        exit(-1);
    }
};

extern Arguments args;
