#include "common.h"

const int LibWebsocketLogLevel = LLL_ERR;

const char *SharedWebsocketDataGlobalVariableName = "SharedWebsocketData";
const int WebsocketBufferCount = 2;
const int WebsocketInitialMessageSize = 1024;

static bool isPluginInitialized = false;

void initPlugin()
{
    if (isPluginInitialized) {
        return;
    }
    lws_set_log_level(LibWebsocketLogLevel, NULL);
    isPluginInitialized = true;
}

void WS_deinitPathHashTable(CSOUND *csound, CS_HASH_TABLE *pathHashTable)
{
    CONS_CELL *pathItem = csound->GetHashTableValues(csound, pathHashTable);
    while (pathItem) {
        WebsocketPath *path = pathItem->value;

        for (int i = 0; i < WebsocketBufferCount; i++) {
            csound->Free(csound, path->messages[i].buffer);
            path->messages[i].size = 0;
        }

        csound->DestroyCircularBuffer(csound, path->messageIndexCircularBuffer);

        csound->Free(csound, path);

        pathItem = pathItem->next;
    }

    csound->DestroyHashTable(csound, pathHashTable);
}

void WS_deinitWebsocket(CSOUND *csound, Websocket *ws)
{
    ws->refCount--;
    if (0 < ws->refCount) {
        return;
    }

    ws->isRunning = false;
    lws_cancel_service(ws->context);

    csound->JoinThread(ws->processThread);

    lws_context_destroy(ws->context);

    WS_deinitPathHashTable(csound, ws->pathFloatsHashTable);
    WS_deinitPathHashTable(csound, ws->pathStringHashTable);

    csound->Free(csound, ws->receiveBuffer);
    csound->Free(csound, ws->protocols);
    csound->Free(csound, ws);
}
