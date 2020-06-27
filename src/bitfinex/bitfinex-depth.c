#include <math.h>
#include <stdbool.h>

#include "bitfinex-depth.h"

#include "../io/strings.h"
#include "../io/time.h"

static int bitfinex_channel_id = 0;
static int bitfinex_count = 0;

bool
fp_equals(double a, double b, double epsilon);

bool
remove_level(Order levels[25], size_t length, double price);

bool
insert_bid_level(Order levels[MAX_ORDER_LEVELS], size_t length, Order level);

bool
insert_ask_level(Order orders[MAX_ORDER_LEVELS], size_t length, Order level);

OrderBookLevel2 *
bitfinex_parse_depth_update(const char *json_string, OrderBookLevel2 *order_book) {
    clock_t t_0 = clock();

    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            lwsl_err("before: \"%s\"\n", error_ptr);
        }
        return NULL;
    }
    // lwsl_user("%s json=%s\n", __func__, json_string);

    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "event");
    if (type != NULL && cJSON_IsString(type) && (type->valuestring != NULL)) {
        if (strcmp("info", type->valuestring) == 0) {
            const cJSON *version = cJSON_GetObjectItemCaseSensitive(root_node, "version");
            const cJSON *status = cJSON_GetObjectItemCaseSensitive(
                    cJSON_GetObjectItemCaseSensitive(root_node, "platform"), "status");
            lwsl_user("%s Welcome: version=%d status=%d\n", __func__, version->valueint, status->valueint);

            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("subscribed", type->valuestring) == 0) {
            const cJSON *channel_name = cJSON_GetObjectItemCaseSensitive(root_node, "channel");
            const cJSON *channel_id = cJSON_GetObjectItemCaseSensitive(root_node, "chanId");
            lwsl_user("%s subscribed: name=\"%s\" id=%d\n", __func__, channel_name->valuestring,
                      (bitfinex_channel_id = channel_id->valueint));

            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("error", type->valuestring) == 0) {
            const cJSON *message = cJSON_GetObjectItemCaseSensitive(root_node, "msg");
            const cJSON *code = cJSON_GetObjectItemCaseSensitive(root_node, "code");
            lwsl_err("%s subscription failure: msg=\"%s\" code=%d\n", __func__, message->valuestring, code->valueint);

            cJSON_Delete(root_node);
            return NULL;
        }
    }

    if (!cJSON_IsArray(root_node)) {
        cJSON_Delete(root_node);
        return NULL;
    }
    cJSON *channel_id = cJSON_GetArrayItem(root_node, 0);
    if (bitfinex_channel_id != channel_id->valueint) {
        lwsl_user("%s wrong channel id=%d\n", __func__, channel_id->valueint);

        cJSON_Delete(root_node);
        return NULL;
    }
    cJSON *first_element = cJSON_GetArrayItem(root_node, 1);
    if (first_element != NULL && cJSON_IsString(first_element) && (first_element->valuestring != NULL)) {
        if (strcmp("hb", first_element->valuestring) == 0) {
            lwsl_debug("%s hb received\n", __func__);

            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("cs", first_element->valuestring) == 0) {
            // cJSON *next_element = cJSON_GetArrayItem(root_node, 2);
            // lwsl_debug("%s cs received=%d\n", __func__, next_element->valueint);

//            bitfinex_depth_check(order_book, next_element->valueint);

            cJSON_Delete(root_node);
            return NULL;
        }
    }
    //OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->time = find_time();

    cJSON *first_item = cJSON_GetArrayItem(first_element, 0);
    if (cJSON_IsArray(first_item)) {
        const cJSON *order_node = NULL;
        // Then this is a snapshot event.
        order_book->bids_length = 0;
        order_book->asks_length = 0;
        cJSON_ArrayForEach(order_node, first_element) {
            cJSON *price = cJSON_GetArrayItem(order_node, 0);
            cJSON *count = cJSON_GetArrayItem(order_node, 1);
            cJSON *amount = cJSON_GetArrayItem(order_node, 2);

            if (amount->valuedouble > 0) {
                if (order_book->bids_length < MAX_ORDER_LEVELS) {
                    Order book = {
                            .price=price->valuedouble,
                            .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble),
                    };
                    order_book->bids[order_book->bids_length++] = book;
                }
            } else {
                if (order_book->asks_length < MAX_ORDER_LEVELS) {
                    Order book = {
                            .price=price->valuedouble,
                            .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble)
                    };
                    order_book->asks[order_book->asks_length++] = book;
                }
            }
        }
        order_book_calculate_mid(order_book);
    } else {
        // This is an update event.
        cJSON *price = cJSON_GetArrayItem(first_element, 0);
        if (fp_equals(price->valuedouble, 0, FP_EPSILON)) {
            lwsl_warn("%s: ZERO price, ignoring", __func__);

            cJSON_Delete(root_node);
            return NULL;
        }

        cJSON *count = cJSON_GetArrayItem(first_element, 1);
        cJSON *amount = cJSON_GetArrayItem(first_element, 2);

        if (count->valueint == 0) {
            // The we must find and remove item
            if (fp_equals(amount->valuedouble, 1, FP_EPSILON)) {
                if (remove_level(order_book->bids, order_book->bids_length, price->valuedouble)) {
                    order_book->bids_length--;
                } else {
                    lwsl_debug("%s: Unable to FIND BID price to remove price=%f, count=%d, amount=%f\n", __func__,
                               price->valuedouble,
                               count->valueint, amount->valuedouble);
                }
            } else if (fp_equals(amount->valuedouble, -1, FP_EPSILON)) {
                if (remove_level(order_book->asks, order_book->asks_length, price->valuedouble)) {
                    order_book->asks_length--;
                } else {
                    lwsl_debug("%s: Unable to FIND ASK price to remove price=%f, count=%d, amount=%f\n", __func__,
                               price->valuedouble,
                               count->valueint, amount->valuedouble);
                }
            } else {
                lwsl_warn("%s: Unexpected value to remove price=%f, count=%d, amount=%f\n", __func__,
                          price->valuedouble,
                          count->valueint, amount->valuedouble);
            }
        } else if (amount->valuedouble > 0) {
            Order book = {.price=price->valuedouble, .amount=fabs(amount->valuedouble)};
            if (insert_bid_level(order_book->bids, order_book->bids_length, book) && order_book->bids_length < MAX_ORDER_LEVELS) {
                order_book->bids_length++;
            }
        } else {
            Order book = {.price=price->valuedouble, .amount=fabs(amount->valuedouble)};
            if (insert_ask_level(order_book->asks, order_book->asks_length, book) && order_book->asks_length < MAX_ORDER_LEVELS) {
                order_book->asks_length++;
            }
        }

        order_book_calculate_mid(order_book);
    }

    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC;
    bitfinex_count++;
    order_book->latency = t_1;
    order_book->id = bitfinex_count;

    cJSON_Delete(root_node);
    return order_book;
}

bool
insert_ask_level(Order levels[MAX_ORDER_LEVELS], size_t length, Order level) {
    size_t i = 0;
    bool inserted = false;
    Order to_insert = level;
    for (; i < length && i < MAX_ORDER_LEVELS; i++) {
        if (inserted || levels[i].price > to_insert.price) {
            // Replace all levels with the next one. Once we inserted what we are looking for.
            Order temp = to_insert;
            to_insert = levels[i];
            levels[i] = temp;
            inserted = true;
        } else if (fp_equals(levels[i].price, to_insert.price, FP_EPSILON)) {
            levels[i] = to_insert;
            return false;
        }
    }
    if (!inserted && i < MAX_ORDER_LEVELS) {
        levels[i] = to_insert;
        inserted = true;
    }
    return inserted;
}

bool
insert_bid_level(Order levels[MAX_ORDER_LEVELS], size_t length, Order level) {
    size_t i = 0;
    bool inserted = false;
    Order to_insert = level;
    for (; i < length && i < MAX_ORDER_LEVELS; i++) {
        if (inserted || levels[i].price < to_insert.price) {
            // Replace all levels with the next one. Once we inserted what we are looking for.
            Order temp = to_insert;
            to_insert = levels[i];
            levels[i] = temp;
            inserted = true;
        } else if (fp_equals(levels[i].price, to_insert.price, FP_EPSILON)) {
            levels[i] = to_insert;
            return false;
        }
    }
    if (!inserted && i < MAX_ORDER_LEVELS) {
        levels[i] = to_insert;
        inserted = true;
    }
    return inserted;
}

bool
fp_equals(double a, double b, double epsilon) {
    if (a == b) {
        return true;
    }
    if (a < b) {
        return (b - a) < epsilon;
    }
    return (a - b) < epsilon;
}

bool
remove_level(Order levels[25], size_t length, double price) {
    bool found = false;
    for (size_t i = 0; i < length && i < MAX_ORDER_LEVELS; i++) {
        if (found || fp_equals(levels[i].price, price, FP_EPSILON)) {
            // Replace all levels with the next one. Once we found what we are looking for.
            if (i + 1 < length) {
                levels[i] = levels[i + 1];
            }
            found = true;
        }
    }
    return found;
}
