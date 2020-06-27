#ifndef BITFINEX_GATEWAY_DATA_H
#define BITFINEX_GATEWAY_DATA_H

#include <libwebsockets.h>

struct client_user_data {
    const char *name; /* should be a constant */

    void (*subscribe)(struct lws *wsi);

    char *(*parse_json)(const void *const in);
};

/* one of these created for each message */

struct msg {
    void *payload; /* is malloc'd */
    size_t len;
};

/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
    struct per_session_data__minimal *pss_list;
    struct lws *wsi;
    uint32_t tail;
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
    struct lws_context *context;
    struct lws_vhost *vhost;
    const struct lws_protocols *protocol;

    struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

    struct lws_ring *ring; /* ringbuffer holding unsent messages */
    struct lws *client_wsi;
};

#endif //BITFINEX_GATEWAY_DATA_H
