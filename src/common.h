#ifndef WEBSOCKET_IO_COMMON_H
#define WEBSOCKET_IO_COMMON_H

#include "csdl.h"

#include <libwebsockets.h>
#include <stdbool.h>


const int LibWebsocketLogLevel;

const char *SharedWebsocketDataGlobalVariableName;
const int WebsocketBufferCount;
const int WebsocketInitialMessageSize;

enum {
    StringType = 1,
    Float64ArrayType = 2
};

typedef struct PortKey
{
    MYFLT port;
    int nullTerminator;
} PortKey;

typedef struct WebsocketMessage {
    char *buffer;
    size_t size;
} WebsocketMessage;

typedef struct WebsocketPath {
    int messageIndex;
    void *messageIndexCircularBuffer;
    WebsocketMessage messages[];
} WebsocketPath;

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

typedef struct {
    CS_HASH_TABLE *portWebsocketHashTable; // key = port float as string, value = Websocket
} SharedWebsocketData;

void initPlugin();
void initPortKey(PortKey *portKey, MYFLT port);

void destroyWebsocket(CSOUND *csound, Websocket *ws);

int32_t resetSharedData(CSOUND *csound, void *vshared);
SharedWebsocketData *getSharedData(CSOUND *csound);

#endif
