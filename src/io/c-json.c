#include "c-json.h"

bool
is_valid_string(const cJSON *node) { return cJSON_IsString(node) && (node->valuestring != NULL); }
