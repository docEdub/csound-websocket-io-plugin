#ifndef WEBSOCKET_IO_GET_H
#define WEBSOCKET_IO_GET_H

#include "common.h"

typedef struct WS_get
{
    OPDS h;
    void *output; // STRINGDAT* or ARRAYDAT*
    MYFLT *port;
    STRINGDAT *path;
    PortKey portKey;
    CSOUND *csound;
    Websocket *websocket;
} WS_get;

int32_t websocket_get_init(CSOUND *csound, WS_get *p);

int32_t websocket_getArray_perf(CSOUND *csound, WS_get *p);
int32_t websocket_getString_perf(CSOUND *csound, WS_get *p);

#endif
