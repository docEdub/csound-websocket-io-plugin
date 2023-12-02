#include "get.h"

int32_t websocket_get_destroy(CSOUND *csound, void *vp)
{
    WS_get *p = vp;
    releaseWebsocket(csound, p->common.websocket);
    return OK;
}

int32_t websocket_get_init(CSOUND *csound, WS_get *p)
{
    initPlugin();

    p->common.csound = csound;

    initPortKey(&p->common.portKey, *p->port);
    p->common.websocket = getWebsocket(csound, *p->port, &p->common);

    csound->RegisterDeinitCallback(csound, p, websocket_get_destroy);

    return OK;
}

int32_t websocket_getArray_perf(CSOUND *csound, WS_get *p) {
    const Websocket *const ws = p->common.websocket;

    CS_HASH_TABLE *hashTable = ws->pathGetFloatsHashTable;

    WebsocketPath *wsPath = csound->GetHashTableValue(csound, hashTable, p->path->data);
    if (!wsPath) {
        return OK;
    }

    ARRAYDAT *output = p->output;

    while (true) {
        int messageIndex = -1;
        const int read = csound->ReadCircularBuffer(csound, wsPath->messageIndexCircularBuffer, &messageIndex, 1);
        if (read == 1) {
            // Make sure we're reading the most recent message sent to the websocket.
            int unused = -1;
            if (csound->PeekCircularBuffer(csound, wsPath->messageIndexCircularBuffer, &unused, 1)) {
                continue;
            }

            MYFLT *d = (MYFLT*) wsPath->messages[messageIndex].buffer;

            size_t size = wsPath->messages[messageIndex].size;
            size_t arrayLength = size / 4;
            if (output->allocated < arrayLength) {
                csound->Free(csound, output->data);
                output->data = csound->Malloc(csound, 2 * size);
                output->allocated = 2 * arrayLength;
            }
            memcpy(output->data, d, size);
        }
        else {
            break;
        }
    }

    return OK;
}

int32_t websocket_getString_perf(CSOUND *csound, WS_get *p) {
    const Websocket *const ws = p->common.websocket;

    CS_HASH_TABLE *hashTable = ws->pathGetStringHashTable;

    WebsocketPath *wsPath = csound->GetHashTableValue(csound, hashTable, p->path->data);
    if (!wsPath) {
        return OK;
    }

    STRINGDAT *output = p->output;

    while (true) {
        int messageIndex = -1;
        const int read = csound->ReadCircularBuffer(csound, wsPath->messageIndexCircularBuffer, &messageIndex, 1);
        if (read == 1) {
            // Make sure we're reading the most recent message sent to the websocket.
            int unused = -1;
            if (csound->PeekCircularBuffer(csound, wsPath->messageIndexCircularBuffer, &unused, 1)) {
                continue;
            }

            char *d = wsPath->messages[messageIndex].buffer;

            // csound->Message(csound, Str("data = %s\n"), d);
            size_t length = strlen(d);
            if (output->size < (int)length) {
                csound->Free(csound, output->data);
                output->data = csound->Malloc(csound, 2 * length + 1);
                output->size = 2 * length + 1;
            }
            memset(output->data, 0, output->size);
            memcpy(output->data, d, length);
        }
        else {
            break;
        }
    }

    return OK;
}
