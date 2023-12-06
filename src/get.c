#include "get.h"

int32_t onWebsocketReceive(struct lws *websocket, void *inputData, size_t inputDataSize)
{
    if (inputData && 0 < inputDataSize) {
        const struct lws_protocols *protocol = lws_get_protocol(websocket);
        Websocket *ws = protocol->user;
        CSOUND *csound = ws->csound;

        char *data = NULL;

        int isFinalFragment = lws_is_final_fragment(websocket);
        if (isFinalFragment && ws->receiveBufferIndex == 0) {
            data = inputData;
        }
        else {
            // Handle partially received messages.
            const int receivedCount = ws->receiveBufferIndex + inputDataSize;
            if (!ws->receiveBuffer) {
                ws->receiveBuffer = csound->Calloc(csound, receivedCount);
                ws->receiveBufferSize = receivedCount;
            }
            else if (ws->receiveBufferSize < receivedCount) {
                char *newReceiveBuffer = csound->Calloc(csound, receivedCount);
                memcpy(newReceiveBuffer, ws->receiveBuffer, ws->receiveBufferSize);
                csound->Free(csound, ws->receiveBuffer);
                ws->receiveBuffer = newReceiveBuffer;
                ws->receiveBufferSize = receivedCount;
            }
            char *receiveBufferDest = ws->receiveBuffer + ws->receiveBufferIndex;
            memcpy(receiveBufferDest, inputData, inputDataSize);
            if (!isFinalFragment) {
                ws->receiveBufferIndex += inputDataSize;
                return OK;
            }
            ws->receiveBufferIndex = 0;

            data = ws->receiveBuffer;
        }

        // Get the path. It should be a null terminated string at the beginning of the received data.
        char *d = data;
        char *path = d;

        size_t pathLength = strlen(path);
        if (pathLength == 0) {
            csound->Message(csound, Str("%s"), "WARNING: websocket path is empty\n");
            return OK;
        }

        // csound->Message(csound, Str("path = %s, "), path);
        d += pathLength + 1;

        // Get the data type. It should be either 1 or 2 for string or doubles array.
        const int8_t type = *d;
        // csound->Message(csound, Str("type = %d, "), type);
        d++;

        size_t bufferSize = 0;
        WebsocketPathData *pathData = NULL;

        // Write the data to the path's messages circular buffer.
        if (Float64ArrayType == type) {
            const uint32_t *length = (uint32_t*)d;
            d += 4;
            // csound->Message(csound, Str("length = %d, "), *length);

            // const double *values = (double*)d;
            // csound->Message(csound, Str("data = %s"), "[ ");
            // csound->Message(csound, Str("%.3f"), values[0]);
            // for (int i = 1; i < *length; i++) {
            //     csound->Message(csound, Str(", %.3f"), values[i]);
            // }
            // csound->Message(csound, Str("%s"), " ]\n");

            pathData = getWebsocketPathData(csound, ws->pathGetFloatsHashTable, path);
            bufferSize = *length * sizeof(double);
        }
        else if (StringType == type) {
            // csound->Message(csound, Str("data = %s\n"), d);

            pathData = getWebsocketPathData(csound, ws->pathGetStringHashTable, path);
            bufferSize = strlen(d) + 1;
        }
        else {
            csound->Message(csound, Str("WARNING: Unknown websocket data type %d received\n"), type);
            return OK;
        }

        WebsocketMessage *msg = pathData->messages + pathData->messageIndex;

        if (0 < msg->size && msg->size < bufferSize) {
            csound->Free(csound, msg->buffer);
            msg->size = 0;
        }
        if (msg->size == 0) {
            msg->buffer = csound->Calloc(csound, bufferSize);
            msg->size = bufferSize;
        }
        memcpy(msg->buffer, d, bufferSize);

        writeWebsocketPathDataMessageIndex(csound, pathData);
    }

    return OK;
}

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

    WebsocketPathData *pathData = csound->GetHashTableValue(csound, hashTable, p->path->data);
    if (!pathData) {
        return OK;
    }

    ARRAYDAT *output = p->output;

    while (true) {
        int messageIndex = -1;
        const int read = csound->ReadCircularBuffer(csound, pathData->messageIndexCircularBuffer, &messageIndex, 1);
        if (read == 1) {
            // Make sure we're reading the most recent message sent to the websocket.
            int unused = -1;
            if (csound->PeekCircularBuffer(csound, pathData->messageIndexCircularBuffer, &unused, 1)) {
                continue;
            }

            WebsocketMessage *msg = pathData->messages + messageIndex;
            MYFLT *d = (MYFLT*) msg->buffer;
            size_t size = msg->size;
            if (output->allocated < size) {
                csound->Free(csound, output->data);
                output->data = csound->Malloc(csound, 2 * size);
                output->allocated = 2 * size;
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

    WebsocketPathData *pathData = csound->GetHashTableValue(csound, hashTable, p->path->data);
    if (!pathData) {
        return OK;
    }

    STRINGDAT *output = p->output;

    while (true) {
        int messageIndex = -1;
        const int read = csound->ReadCircularBuffer(csound, pathData->messageIndexCircularBuffer, &messageIndex, 1);
        if (read == 1) {
            // Make sure we're reading the most recent message sent to the websocket.
            int unused = -1;
            if (csound->PeekCircularBuffer(csound, pathData->messageIndexCircularBuffer, &unused, 1)) {
                continue;
            }

            char *d = pathData->messages[messageIndex].buffer;

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
