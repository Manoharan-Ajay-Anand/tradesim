#include <doctest/doctest.h>

#include <market_types.hpp>

#include <limits>

#define NUM_ORDERS 4

TEST_CASE("bid queue test") {
    tradesim::bid_queue bids;
    tradesim::object_id trader_id;

    SUBCASE("different prices") {
        tradesim::order o { 0, trader_id, tradesim::order_type::BID, 0, 10 };
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            o.m_id = NUM_ORDERS - i + 1;
            o.m_price = i;
            bids[{o.m_id, o.m_price}] = o;
        }
        long price_compare = std::numeric_limits<long>::max();
        for (auto it = bids.begin(); it != bids.end(); ++it) {
            auto& order = it->second;
            CHECK(order.m_price < price_compare);
            price_compare = order.m_price;
        }
    }

    SUBCASE("same prices") {
        tradesim::order o { 0, trader_id, tradesim::order_type::BID, 1, 10 };
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            o.m_id = NUM_ORDERS - i + 1;
            bids[{o.m_id, o.m_price}] = o;
        }
        long id_compare = 0;
        for (auto it = bids.begin(); it != bids.end(); ++it) {
            auto& order = it->second;
            CHECK(order.m_id > id_compare);
            id_compare = order.m_id;
        }
    }

    SUBCASE("order key equivalence") {
        tradesim::order o { 1, trader_id, tradesim::order_type::BID, 1, 10 };
        bids[{o.m_id, o.m_price}] = o;
        auto it = bids.find({o.m_id, 0});
        CHECK(it != bids.end());
        bids.erase({o.m_id, 0});
        it = bids.find({o.m_id, 0});
        CHECK(it == bids.end());
    }
}

TEST_CASE("ask queue test") {
    tradesim::ask_queue asks;
    tradesim::object_id trader_id;

    SUBCASE("different prices") {
        tradesim::order o { 0, trader_id, tradesim::order_type::ASK, 0, 10 };
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            o.m_id = NUM_ORDERS - i + 1;
            o.m_price = i;
            asks[{o.m_id, o.m_price}] = o;
        }
        long price_compare = 0;
        for (auto it = asks.begin(); it != asks.end(); ++it) {
            auto& order = it->second;
            CHECK(order.m_price > price_compare);
            price_compare = order.m_price;
        }
    }

    SUBCASE("same prices") {
        tradesim::order o { 0, trader_id, tradesim::order_type::ASK, 1, 10 };
        for (long i = 1; i <= NUM_ORDERS; ++i) {
            o.m_id = NUM_ORDERS - i + 1;
            asks[{o.m_id, o.m_price}] = o;
        }
        long id_compare = 0;
        for (auto it = asks.begin(); it != asks.end(); ++it) {
            auto& order = it->second;
            CHECK(order.m_id > id_compare);
            id_compare = order.m_id;
        }
    }

    SUBCASE("order key equivalence") {
        tradesim::order o { 1, trader_id, tradesim::order_type::ASK, 1, 10 };
        asks[{o.m_id, o.m_price}] = o;
        auto it = asks.find({o.m_id, 0});
        CHECK(it != asks.end());
        asks.erase({o.m_id, 0});
        it = asks.find({o.m_id, 0});
        CHECK(it == asks.end());
    }
}
