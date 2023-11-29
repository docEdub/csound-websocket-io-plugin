#include "set.h"

int32_t websocket_set_destroy(CSOUND *csound, void *vp)
{
    WS_set *p = vp;
    destroyWebsocket(csound, p->common.websocket);
    return OK;
}

int32_t websocket_set_init(CSOUND *csound, WS_set *p)
{
    initPlugin();

    p->common.csound = csound;

    initPortKey(&p->common.portKey, *p->port);
    p->common.websocket = createWebsocket(csound, *p->port, &p->common);

    csound->RegisterDeinitCallback(csound, p, websocket_set_destroy);

    return OK;
}

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->common.websocket;
    (void)ws;

    return OK;
}

int32_t websocket_setString_perf(CSOUND *csound, WS_set *p) {
    (void)csound;

    const Websocket *const ws = p->common.websocket;
    (void)ws;

    return OK;
}
