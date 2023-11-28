#include "set.h"

// TODO: deduplicate this with the same function in get.c.
static char *initPortKeyString(MYFLT port, PortKey *portKey)
{
    portKey->port = port;
    portKey->nullTerminator = 0;

    char *s = (char*)(&portKey->port);
    for (size_t i = 0; i < sizeof(MYFLT); i++) {
        if (s[i] == 0) {
            s[i] = '~';
        }
    }

    return s;
}

static int32_t WS_callback(
    struct lws *websocket,
    enum lws_callback_reasons reason,
    void * user,
    void *inputData,
    size_t inputDataSize)
{
    (void)websocket;
    (void)reason;
    (void)user;
    (void)inputData;
    (void)inputDataSize;
    return OK;
}

static uintptr_t WS_processThread(void *vws)
{
    Websocket *ws = vws;
    ws->isRunning = true;

    while (ws->isRunning) {
        // lws_service(ws->context, 0);
    }

    return 0;
}

static Websocket *WS_initWebsocket(CSOUND *csound, MYFLT port, char *portKey)
{
    (void)portKey;

    Websocket *ws = NULL;

    // SharedWebsocketData *shared = WS_getSharedData(csound);

    // char *hashTablePortKey = csound->GetHashTableKey(csound, shared->portWebsocketHashTable, portKey);
    // if (hashTablePortKey) {
    //     ws = csound->GetHashTableValue(csound, shared->portWebsocketHashTable, hashTablePortKey);
    //     ws->refCount++;
    //     return ws;
    // }

    ws = csound->Calloc(csound, sizeof(Websocket));
    ws->csound = csound;
    ws->pathStringHashTable = csound->CreateHashTable(csound);
    ws->pathFloatsHashTable = csound->CreateHashTable(csound);
    ws->refCount = 1;

    // csound->SetHashTableValue(csound, shared->portWebsocketHashTable, portKey, ws);

    // Allocate 2 protocols; the actual protocol, and a null protocol at the end
    // (idk why, but this is how the original websocket opcode does it and the call to lws_service sometimes crashes
    // without it).
    ws->protocols = csound->Calloc(csound, sizeof(struct lws_protocols) * 2);

    ws->protocols[0].callback = WS_callback;
    ws->protocols[0].id = 1000;
    ws->protocols[0].name = "csound";
    ws->protocols[0].per_session_data_size = sizeof(void*);
    ws->protocols[0].user = ws;

    ws->info.port = port;
    ws->info.protocols = ws->protocols;
    ws->info.gid = -1;
    ws->info.uid = -1;

    ws->context = lws_create_context(&ws->info);
    if (UNLIKELY(ws->context == NULL)) {
        csound->InitError(csound, Str("cannot start websocket on port %d\n"), port);
    }

    ws->processThread = csound->CreateThread(WS_processThread, ws);

    return ws;
}

int32_t websocket_set_deinit(CSOUND *csound, void *vp)
{
    WS_set *p = vp;
    WS_deinitWebsocket(csound, p->websocket);
    return OK;
}

int32_t websocket_set_init(CSOUND *csound, WS_set *p)
{
    initPlugin();

    p->csound = csound;

    char *portKey = initPortKeyString(*p->port, &p->portKey);
    p->websocket = WS_initWebsocket(csound, *p->port, portKey);

    csound->RegisterDeinitCallback(csound, p, websocket_set_deinit);

    return OK;
}

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->websocket;
    (void)ws;

    return OK;
}

int32_t websocket_setString_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->websocket;
    (void)ws;

    return OK;
}
