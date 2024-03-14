#ifndef TRADESIM_MARKET_TYPES_HPP
#define TRADESIM_MARKET_TYPES_HPP

#include "types.hpp"

#include <map>
#include <vector>

namespace tradesim {

enum class order_type {
    BID = 1,
    ASK = -1
};

struct order_key {
    long m_id;
    long m_price;
};

struct bid_compare {
    constexpr bool operator()(const order_key& lhs, const order_key& rhs) const {
        return lhs.m_id != rhs.m_id &&
              (lhs.m_price > rhs.m_price || (lhs.m_price == rhs.m_price && lhs.m_id < rhs.m_id));
    }
};

struct ask_compare {
    constexpr bool operator()(const order_key& lhs, const order_key& rhs) const {
        return lhs.m_id != rhs.m_id &&
              (lhs.m_price < rhs.m_price || (lhs.m_price == rhs.m_price && lhs.m_id < rhs.m_id));
    }
};

struct order {
    long m_id;
    object_id m_trader;
    order_type m_type;
    long m_price;
    long m_quantity;
};

using bid_queue = std::map<order_key, order, bid_compare>;

using ask_queue = std::map<order_key, order, ask_compare>;

struct price_point {
    long m_bid_count = 0;
    long m_ask_count = 0;
};

class position {
private:
    long m_pnl = 0;
    long m_count = 0;

public:
    long get_pnl() const;

    long get_count() const;

    void confirm(order_type type, long price, long quantity);
};

}

#endif
