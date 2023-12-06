#include "set.h"

static void sendWebsocketPathData(CSOUND *csound, struct lws *websocket, WebsocketPathData *pathData)
{
    while (true) {
        int messageIndex = -1;
        const int read = csound->ReadCircularBuffer(csound, pathData->messageIndexCircularBuffer, &messageIndex, 1);
        if (read == 1) {
            // Make sure we're reading the most recent message to send to the websocket.
            int unused = -1;
            if (csound->PeekCircularBuffer(csound, pathData->messageIndexCircularBuffer, &unused, 1)) {
                continue;
            }

            WebsocketMessage *msg = pathData->messages + messageIndex;
            lws_write(websocket, (unsigned char*) msg->buffer, msg->size, LWS_WRITE_BINARY);
        }
        else {
            break;
        }
    }
}

static void sendWebsocketPathHashTable(CSOUND *csound, struct lws *websocket, CS_HASH_TABLE *pathHashTable)
{
    CONS_CELL *pathItem = csound->GetHashTableValues(csound, pathHashTable);
    while (pathItem) {
        WebsocketPathData *pathData = pathItem->value;
        sendWebsocketPathData(csound, websocket, pathData);
        pathItem = pathItem->next;
    }
}

int32_t onWebsocketWritable(struct lws *websocket)
{
    const struct lws_protocols *protocol = lws_get_protocol(websocket);
    Websocket *ws = protocol->user;
    CSOUND *csound = ws->csound;

    sendWebsocketPathHashTable(csound, websocket, ws->pathSetFloatsHashTable);
    sendWebsocketPathHashTable(csound, websocket, ws->pathSetStringHashTable);

    return OK;
}

int32_t websocket_set_destroy(CSOUND *csound, void *vp)
{
    WS_set *p = vp;
    releaseWebsocket(csound, p->common.websocket);
    return OK;
}

int32_t websocket_set_init(CSOUND *csound, WS_set *p)
{
    initPlugin();

    p->common.csound = csound;

    initPortKey(&p->common.portKey, *p->port);
    p->common.websocket = getWebsocket(csound, *p->port, &p->common);

    csound->RegisterDeinitCallback(csound, p, websocket_set_destroy);

    size_t pathLength = strlen(p->path->data);

    const CS_TYPE *type = csound->GetTypeForArg(p->input);
    const char *typeName = type->varTypeName;
    const uint8_t dataType = ('S' == typeName[0]) ? StringType : Float64ArrayType;

    p->msgPreSize = pathLength + 2;
    if (Float64ArrayType == dataType) {
        p->msgPreSize += (4 - (p->msgPreSize % 4)) % 4; // Align to size of uint32 for array length.
        p->msgPreArraySizeIndex = p->msgPreSize;
        p->msgPreSize += 4;
        p->msgPreSize += (8 - (p->msgPreSize % 8)) % 8; // Align to size of double for array length. TODO: Use size of MYFLT instead of double.
    }

    p->msgPre = csound->Calloc(csound, p->msgPreSize);

    char *d = p->msgPre;
    memcpy(d, p->path->data, strlen(p->path->data));
    d += pathLength + 1;

    *d = dataType;

    return OK;
}

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p) {
    const Websocket *const ws = p->common.websocket;
    char *path = p->path->data;
    ARRAYDAT *input = p->input;

    // Write websocket data to path's message buffer and update messageIndex circular buffer ...

    WebsocketPathData *pathData = getWebsocketPathData(csound, ws->pathSetFloatsHashTable, path);

    const size_t inputSize = input->allocated;
    const size_t msgSize = p->msgPreSize + inputSize;

    WebsocketMessage *msg = pathData->messages + pathData->messageIndex;
    if (msg->size < msgSize) {
        csound->Free(csound, msg->buffer);
        msg->buffer = csound->Malloc(csound, 2 * msgSize);
        msg->size = msgSize;
    }
    memcpy(msg->buffer, p->msgPre, p->msgPreSize);
    *(uint32_t*)(msg->buffer + p->msgPreArraySizeIndex) = inputSize / sizeof(MYFLT);
    memcpy(msg->buffer + p->msgPreSize, input->data, inputSize);

    writeWebsocketPathDataMessageIndex(csound, pathData);

    return OK;
}

int32_t websocket_setString_perf(CSOUND *csound, WS_set *p) {
    const Websocket *const ws = p->common.websocket;
    char *path = p->path->data;
    STRINGDAT *input = p->input;

    // Write websocket data to path's message buffer and update messageIndex circular buffer ...

    WebsocketPathData *pathData = getWebsocketPathData(csound, ws->pathSetFloatsHashTable, path);

    const size_t inputSize = input->size;
    const size_t msgSize = p->msgPreSize + inputSize;

    WebsocketMessage *msg = pathData->messages + pathData->messageIndex;
    if (msg->size < msgSize) {
        csound->Free(csound, msg->buffer);
        msg->buffer = csound->Malloc(csound, 2 * msgSize);
        msg->size = msgSize;
    }
    memcpy(msg->buffer, p->msgPre, p->msgPreSize);
    memcpy(msg->buffer + p->msgPreSize, input->data, inputSize);

    writeWebsocketPathDataMessageIndex(csound, pathData);

    return OK;
}
