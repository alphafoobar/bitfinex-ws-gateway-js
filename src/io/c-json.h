#ifndef BITFINEX_GATEWAY_C_JSON_H
#define BITFINEX_GATEWAY_C_JSON_H

#include <stdbool.h>
#include "cjson/cJSON.h"

bool
is_valid_string(const cJSON *node);

#endif //BITFINEX_GATEWAY_C_JSON_H
