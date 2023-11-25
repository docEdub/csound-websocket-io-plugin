#ifndef WEBSOCKET_IO_SET_H
#define WEBSOCKET_IO_SET_H

#include "common.h"

typedef struct WS_set
{
    OPDS h;
    MYFLT *port;
    STRINGDAT *path;
    void *input; // STRINGDAT* or ARRAYDAT*
    PortKey portKey;
    CSOUND *csound;
    Websocket *websocket;
} WS_set;

int32_t websocket_set_init(CSOUND *csound, WS_set *p);

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p);
int32_t websocket_setString_perf(CSOUND *csound, WS_set *p);

#endif
