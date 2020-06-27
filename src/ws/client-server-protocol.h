#ifndef BITFINEX_GATEWAY_CLIENT_SERVER_PROTOCOL_H
#define BITFINEX_GATEWAY_CLIENT_SERVER_PROTOCOL_H

#include <libwebsockets.h>

#include "data.h"

int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                 void *user, void *in, size_t len);

int
websocket_write_back(struct lws *wsi_in, const char *str);

#define LWS_PLUGIN_PROTOCOL_MINIMAL \
        {  \
                "default", \
                callback_minimal, \
                sizeof(struct per_session_data__minimal), \
                10240, \
                1000, \
                NULL, \
                0 \
        }

#endif //BITFINEX_GATEWAY_CLIENT_SERVER_PROTOCOL_H
