#ifndef TRADESIM_MARKET_HPP
#define TRADESIM_MARKET_HPP

#include "types.hpp"
#include "market_types.hpp"
#include "broadcast.hpp"

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <memory>

namespace tradesim {

using order_map = std::unordered_map<long, order>;

class market {
private:
    std::unordered_map<object_id, position> m_accounts;
    broadcast m_broadcast;

    std::unordered_map<long, price_point> m_price_points;

    trade m_last_trade;

    std::unordered_map<object_id, std::unordered_set<long>> m_trader_orders;

    long m_order_count = 0;
    order_map m_orders;
    
    std::map<long, std::set<long>, std::greater<long>> m_bids;
    std::map<long, std::set<long>> m_asks;

    void update_bid_ask_count(long price, long bid_diff, long ask_diff);

    void update_bid_count(long price, long diff);
    void update_ask_count(long price, long diff);

    void update_accounts(const object_id& buyer_id, const object_id& seller_id, trade t);

    void update_order(order& o, trade t, std::set<long>& order_set);

    void execute_trades();
public:
    market() = default;

    market(const market&) = delete;
    market& operator=(const market&) = delete;

    market(market&&) = delete;
    market& operator=(market&&) = delete;

    void register_account(const object_id& trader_id);

    bool subscribe(const object_id& trader_id, market_stream* m_ptr);
    void unsubscribe(const object_id& trader_id);

    void place_bid(const object_id& trader_id, long price, long quantity);
    void place_ask(const object_id& trader_id, long price, long quantity);

    bool cancel_order(const object_id& trader_id, long order_id);
};

}

#endif
