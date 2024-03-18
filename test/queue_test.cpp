#include <doctest/doctest.h>

#include <market_types.hpp>

#include <limits>

#define NUM_ORDERS 4

TEST_CASE("bid queue test") {
    tradesim::bid_queue bids;
    tradesim::object_id trader_id;

    SUBCASE("different prices") {
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            bids.push({ NUM_ORDERS - i + 1, i });
        }
        long price_compare = std::numeric_limits<long>::max();
        while (!bids.empty()) {
            auto& key = bids.top();
            CHECK(key.m_price < price_compare);
            price_compare = key.m_price;
            bids.pop();
        }
    }

    SUBCASE("same prices") {
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            bids.push({NUM_ORDERS - i + 1, 10});
        }
        long id_compare = 0;
        while (!bids.empty()) {
            auto& key = bids.top();
            CHECK(key.m_id > id_compare);
            id_compare = key.m_id;
            bids.pop();
        }
    }
}

TEST_CASE("ask queue test") {
    tradesim::ask_queue asks;
    tradesim::object_id trader_id;

    SUBCASE("different prices") {
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            asks.push({ NUM_ORDERS - i + 1, i });
        }
        long price_compare = 0;
        while (!asks.empty()) {
            auto& key = asks.top();
            CHECK(key.m_price > price_compare);
            price_compare = key.m_price;
            asks.pop();
        }
    }

    SUBCASE("same prices") {
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            asks.push({NUM_ORDERS - i + 1, 10});
        }
        long id_compare = 0;
        while (!asks.empty()) {
            auto& key = asks.top();
            CHECK(key.m_id > id_compare);
            id_compare = key.m_id;
            asks.pop();
        }
    }
}
