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

struct order {
    long m_id;
    object_id m_trader;
    order_type m_type;
    long m_price;
    mutable long m_quantity;
};

struct bid_compare {
    constexpr bool operator()(const order& lhs, const order& rhs) const {
        return rhs.m_price > lhs.m_price || (rhs.m_price == lhs.m_price && rhs.m_id < lhs.m_id);
    }
};

struct ask_compare {
    constexpr bool operator()(const order& lhs, const order& rhs) const {
        return rhs.m_price < lhs.m_price || (rhs.m_price == lhs.m_price && rhs.m_id < lhs.m_id);
    }
};

using bid_queue = std::priority_queue<order, std::vector<order>, bid_compare>;

using ask_queue = std::priority_queue<order, std::vector<order>, ask_compare>;

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
