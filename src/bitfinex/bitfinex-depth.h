#ifndef BITFINEX_GATEWAY_BITFINEX_DEPTH_H
#define BITFINEX_GATEWAY_BITFINEX_DEPTH_H

#define FP_EPSILON 0.01

#include "cjson/cJSON.h"
#include "../depth.h"
#include "../ws/client-server-protocol.h"

OrderBookLevel2 *
bitfinex_parse_depth_update(const char *json_string, OrderBookLevel2 *ptr);

#endif //BITFINEX_GATEWAY_BITFINEX_DEPTH_H
