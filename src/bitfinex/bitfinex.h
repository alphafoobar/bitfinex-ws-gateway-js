#ifndef BITFINEX_GATEWAY_BITFINEX_H
#define BITFINEX_GATEWAY_BITFINEX_H

#include "../ws/data.h"

int
bitfinex_connect_client(const struct per_vhost_data__minimal *vhd);

#endif //BITFINEX_GATEWAY_BITFINEX_H
