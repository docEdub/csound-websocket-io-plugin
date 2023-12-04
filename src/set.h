#ifndef WEBSOCKET_IO_SET_H
#define WEBSOCKET_IO_SET_H

#include "common.h"

typedef struct WS_set
{
    OPDS h;
    MYFLT *port;
    STRINGDAT *path;
    void *input; // STRINGDAT* or ARRAYDAT*
    WS_common common;
    char *msgPre; // path + '\0 + data type + 4 byte alignment padding + uint32 array size + 8 byte alignment padding.
    size_t msgPreArraySizeIndex;
    size_t msgPreSize;
} WS_set;

int32_t websocket_set_init(CSOUND *csound, WS_set *p);

int32_t websocket_setArray_perf(CSOUND *csound, WS_set *p);
int32_t websocket_setString_perf(CSOUND *csound, WS_set *p);

#endif
