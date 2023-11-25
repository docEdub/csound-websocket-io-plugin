#include "set.h"

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
