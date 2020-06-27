#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>
#include "bitfinex.h"
#include "../ws/client-server-protocol.h"
#include "../ws/client-connect.h"
#include "../depth.h"
#include "bitfinex-depth.h"

static void
bitfinex_subscribe(struct lws *wsi_in) {
    lwsl_user("%s\n", __func__);
//    websocket_write_back(wsi_in, "{\"event\": \"conf\", \"flags\": 131072 }");
    const char *m = "{\"event\": \"subscribe\",\"channel\": \"book\",\"symbol\": \"tBTCUSD\",\"prec\": \"P0\", \"freq\": \"F0\", \"len\": 25}";
    websocket_write_back(wsi_in, m);
}

static OrderBookLevel2 _order_book;
static char _message[200];
        
static char *
bitfinex_parse(const char *const json_string) {
    OrderBookLevel2 *book = bitfinex_parse_depth_update(json_string, &_order_book);
    if (book != NULL) {
        sprintf(_message, "{\"e\":\"bitfinex\",\"s\":\"btc/usd\",\"b\":%f,\"m\":%f,\"a\":%f,\"t\":%f,\"i\":%ld}",
                book->bids[0].price,
                book->mid,
                book->asks[0].price,
                book->time,
                book->id);
        // Don't free the order book, because it is just a reference to the static order book we have above.
        return _message;
    }
    return NULL;
}

int
bitfinex_connect_client(const struct per_vhost_data__minimal *vhd) {
    _order_book.exchange = strndup("bitfinex", 20);
    _order_book.market_name = strndup("btc/usd", 20);

    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "bitfinex";
    userdata->subscribe = bitfinex_subscribe;
    userdata->parse_json = (char *(*)(const void *const)) bitfinex_parse;
    return connect_client(vhd, 443, "api.bitfinex.com", "/ws/2", userdata);
}

