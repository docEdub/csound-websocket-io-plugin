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

            WebsocketMessage *message = pathData->messages + messageIndex;
            lws_write(websocket, (unsigned char *)message->buffer, message->size, LWS_WRITE_BINARY);
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

    sendWebsocketPathHashTable(csound, websocket, ws->pathGetFloatsHashTable);
    sendWebsocketPathHashTable(csound, websocket, ws->pathGetStringHashTable);

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

    return OK;
}

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->common.websocket;

    // TODO: Write websocket data to path's message buffer and update messageIndex circular buffer.
    (void)ws;

    return OK;
}

int32_t websocket_setString_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->common.websocket;

    // TODO: Write websocket data to path's message buffer and update messageIndex circular buffer.
    (void)ws;

    return OK;
}
