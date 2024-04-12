#ifndef TRADESIM_MARKET_TYPES_HPP
#define TRADESIM_MARKET_TYPES_HPP

#include "types.hpp"

#include <queue>
#include <vector>

namespace tradesim {

enum class order_type {
    BID = 1,
    ASK = -1
};

void from_json(const json& j, order_type& type);

void to_json(json& j, const order_type& type);

struct order_form {
    object_id m_market_id;
    object_id m_trader_id;
    order_type m_type;
    long m_price;
    long m_quantity;
};

void from_json(const json& j, order_form& o);

struct order_key {
    long m_id;
    long m_price;
};

struct bid_compare {
    constexpr bool operator()(const order_key& lhs, const order_key& rhs) const {
        return rhs.m_price > lhs.m_price || (rhs.m_price == lhs.m_price && rhs.m_id < lhs.m_id);
    }
};

struct ask_compare {
    constexpr bool operator()(const order_key& lhs, const order_key& rhs) const {
        return rhs.m_price < lhs.m_price || (rhs.m_price == lhs.m_price && rhs.m_id < lhs.m_id);
    }
};

struct order {
    long m_id;
    object_id m_trader;
    order_type m_type;
    long m_price;
    long m_quantity;
};

using bid_queue = std::priority_queue<order_key, std::vector<order_key>, bid_compare>;

using ask_queue = std::priority_queue<order_key, std::vector<order_key>, ask_compare>;

struct price_point {
    long m_price;
    long m_bid_count;
    long m_ask_count;
};

void to_json(json& j, const price_point& pp);

struct position {
    long m_pnl = 0;
    long m_count = 0;

    void confirm(order_type type, long price, long quantity);
};

void to_json(json& j, const position& pos);

struct trade {
    bool m_matched;
    long m_price;
    long m_quantity;

    static trade match(const order& bid, const order& ask);
};

void to_json(json& j, const trade& t);

struct order_update {
    long m_order_id;
    long m_order_type;
    long m_price;
    long m_quantity;
    long m_remaining;
};

void to_json(json& j, const order_update& o);

}

#endif
