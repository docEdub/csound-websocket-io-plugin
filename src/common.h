#ifndef WEBSOCKET_IO_COMMON_H
#define WEBSOCKET_IO_COMMON_H

#include "csdl.h"

#include <libwebsockets.h>
#include <stdbool.h>


const int LibWebsocketLogLevel;

const char *SharedWebsocketDataGlobalVariableName;
const int WebsocketBufferCount;
const int WebsocketInitialMessageSize;

typedef struct Websocket {
    CSOUND *csound;
    CS_HASH_TABLE *pathFloatsHashTable; // key = path string, value = WebsocketPath containing a MYFLT array.
    CS_HASH_TABLE *pathStringHashTable; // key = path string, value = WebsocketPath containing a string
    int refCount;
    struct lws_context *context;
    struct lws_protocols *protocols;
    struct lws_context_creation_info info;
    char *receiveBuffer;
    int receiveBufferSize;
    int receiveBufferIndex;
    void *processThread;
    bool isRunning;
} Websocket;

typedef struct WebsocketMessage {
    char *buffer;
    size_t size;
} WebsocketMessage;

typedef struct WebsocketPath {
    int messageIndex;
    void *messageIndexCircularBuffer;
    WebsocketMessage messages[];
} WebsocketPath;

typedef struct PortKey
{
    MYFLT port;
    int nullTerminator;
} PortKey;

void initPlugin();

void WS_deinitWebsocket(CSOUND *csound, Websocket *ws);

#endif
