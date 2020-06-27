#ifndef BITFINEX_GATEWAY_DEPTH_H
#define BITFINEX_GATEWAY_DEPTH_H

typedef struct order {
    double price;
    double amount;
} Order;

#ifndef EMPTY_ORDER
#define EMPTY_ORDER (Order){.price=-1.0, .amount=-1.0}
#endif //EMPTY_ORDER

#define MAX_ORDER_LEVELS 25

typedef struct {
    char *exchange;
    char *market_name;
    double market;
    double time;
    double mid;
    double latency;
    long id;
    unsigned int bids_length;
    Order bids[MAX_ORDER_LEVELS];
    unsigned int asks_length;
    Order asks[MAX_ORDER_LEVELS];
} OrderBookLevel2;

void
order_book_calculate_mid(OrderBookLevel2 *order_book);

void
order_book_delete(OrderBookLevel2 *order_book);

#endif //BITFINEX_GATEWAY_DEPTH_H
