#include "set.h"

static int32_t callback(
    struct lws *websocket,
    enum lws_callback_reasons reason,
    void * user,
    void *inputData,
    size_t inputDataSize)
{
    IGN(websocket);
    IGN(user);
    IGN(inputData);
    IGN(inputDataSize);

    if (reason != LWS_CALLBACK_SERVER_WRITEABLE) {
        return OK;
    }

    return OK;
}

static uintptr_t processThread(void *vws)
{
    Websocket *ws = vws;
    ws->isRunning = true;

    while (ws->isRunning) {
      // https://libwebsockets.org/lws-api-doc-main/html/group__callback-when-writeable.html#gabbe4655c7eeb3eb1671b2323ec6b3107
      //
      // Request a callback for all connections using the given protocol when it becomes possible to write to each socket without blocking in turn.
      //  context:  Websocket context
      //  protocol:  Protocol whose connections will get callbacks
      lws_callback_on_writable_all_protocol(ws->context, &ws->protocols[0]);
    }

    return 0;
}

static Websocket *createWebsocket(CSOUND *csound, WS_set *p)
{
    Websocket *ws = NULL;

    SharedWebsocketData *shared = getSharedData(csound);

    char *hashTablePortKey = csound->GetHashTableKey(csound, shared->portWebsocketHashTable, (char*)&p->portKey);
    if (hashTablePortKey) {
        ws = csound->GetHashTableValue(csound, shared->portWebsocketHashTable, hashTablePortKey);
        ws->refCount++;
        return ws;
    }

    ws = csound->Calloc(csound, sizeof(Websocket));
    ws->csound = csound;
    ws->pathStringHashTable = csound->CreateHashTable(csound);
    ws->pathFloatsHashTable = csound->CreateHashTable(csound);
    ws->refCount = 1;

    csound->SetHashTableValue(csound, shared->portWebsocketHashTable, (char*)&p->portKey, ws);

    // Allocate 2 protocols; the actual protocol, and a null protocol at the end
    // (idk why, but this is how the original websocket opcode does it and the call to lws_service sometimes crashes
    // without it).
    ws->protocols = csound->Calloc(csound, sizeof(struct lws_protocols) * 2);

    ws->protocols[0].callback = callback;
    ws->protocols[0].id = 1000;
    ws->protocols[0].name = "csound";
    ws->protocols[0].per_session_data_size = sizeof(void*);
    ws->protocols[0].user = ws;

    ws->info.port = *p->port;
    ws->info.protocols = ws->protocols;
    ws->info.gid = -1;
    ws->info.uid = -1;

    ws->context = lws_create_context(&ws->info);
    if (UNLIKELY(ws->context == NULL)) {
        csound->InitError(csound, Str("cannot start websocket on port %d\n"), *p->port);
    }

    ws->processThread = csound->CreateThread(processThread, ws);

    return ws;
}

int32_t websocket_set_destroy(CSOUND *csound, void *vp)
{
    WS_set *p = vp;
    destroyWebsocket(csound, p->websocket);
    return OK;
}

int32_t websocket_set_init(CSOUND *csound, WS_set *p)
{
    initPlugin();

    p->csound = csound;

    initPortKey(&p->portKey, *p->port);
    p->websocket = createWebsocket(csound, p);

    csound->RegisterDeinitCallback(csound, p, websocket_set_destroy);

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
