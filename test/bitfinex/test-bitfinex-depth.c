#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "bitfinex/bitfinex-depth.h"
#include "../file.h"

static void
test_binance_parse_info() {
    char *depth_json = read_file("resources/bitfinex-info.json", 103);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);
    assert_null(actual);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_binance_parse_subbed() {
    char *depth_json = read_file("resources/bitfinex-subbed.json", 125);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    assert_null(actual);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_binance_parse_depth_snapshot_btcusd() {
    char *depth_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->bids[1].price, 10195, 0.01);
    assert_float_equal(actual->bids[1].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_binance_mid() {
    char *depth_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.001);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_binance_parse_depth_update_btcusd() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    char *depth_json = read_file("resources/bitfinex-update.json", 22);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->asks[23].price, 10221, 0.01);
    assert_float_equal(actual->asks[23].amount, 2.69390221, 0.01);

    free(snapshot_json);
    free(depth_json);
//    order_book_delete(actual);
}

static void
test_binance_parse_depth_update_btcusd_inside() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.0001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10197,0,-1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.999, 0.0001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10198, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[23].price, 10222, 0.01);
    assert_float_equal(actual->asks[23].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_binance_parse_depth_update_btcusd_inside_bids() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10195,0,1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.0001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 24);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->bids[23].price, 10172, 0.01);
    assert_float_equal(actual->bids[23].amount, 0.976, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[24].price, 10222, 0.01);
    assert_float_equal(actual->asks[24].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_binance_remove_best_bid() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10196,0,1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10196.166, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 24);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10195, 0.01);
    assert_float_equal(actual->bids[0].amount, 0.001, 0.01);
    assert_float_equal(actual->bids[23].price, 10172, 0.01);
    assert_float_equal(actual->bids[23].amount, 0.976, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);
    assert_float_equal(actual->asks[24].price, 10222, 0.01);
    assert_float_equal(actual->asks[24].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_binance_remove_best_ask() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10197,0,-1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.999, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10198, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[23].price, 10222, 0.01);
    assert_float_equal(actual->asks[23].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_binance_parse_info),
            cmocka_unit_test(test_binance_parse_subbed),
            cmocka_unit_test(test_binance_parse_depth_snapshot_btcusd),
            cmocka_unit_test(test_binance_parse_depth_update_btcusd_inside),
            cmocka_unit_test(test_binance_parse_depth_update_btcusd_inside_bids),
            cmocka_unit_test(test_binance_remove_best_bid),
            cmocka_unit_test(test_binance_remove_best_ask),
            cmocka_unit_test(test_binance_mid),
            cmocka_unit_test(test_binance_parse_depth_update_btcusd),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
