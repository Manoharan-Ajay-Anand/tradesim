#ifndef TRADESIM_MARKET_HPP
#define TRADESIM_MARKET_HPP

#include "types.hpp"
#include "market_types.hpp"
#include "broadcast.hpp"

#include <cppevent_base/task.hpp>
#include <cppevent_base/async_queue.hpp>

#include <unordered_map>
#include <memory>

namespace cppevent {

class event_loop;

}

namespace tradesim {

class market {
private:
    std::unordered_map<object_id, position> m_accounts;
    broadcast m_broadcast;

    std::unordered_map<long, price_point> m_price_points;

    long m_order_count;
    std::unordered_map<long, order> m_orders;
    bid_queue m_bids;
    ask_queue m_asks;

    cppevent::async_queue<message> m_messages;
    cppevent::awaitable_task<void> m_task;

    cppevent::awaitable_task<void> broadcast_messages();

    void update_bid_count(long price, long diff);
    void update_ask_count(long price, long diff);

    void update_account(const object_id& trader_id, order_type type, trade t);

    void execute_trades();
public:
    market(cppevent::event_loop& el);

    market(const market&) = delete;
    market& operator=(const market&) = delete;

    market(market&&) = delete;
    market& operator=(market&&) = delete;

    void register_account(const object_id& trader_id);

    std::unique_ptr<subscription> subscribe(const object_id& trader_id, market_stream* m_ptr);
    void unsubscribe(const object_id& trader_id);

    void place_bid(const object_id& trader_id, long price, long quantity);
    void place_ask(const object_id& trader_id, long price, long quantity);
};

}

#endif
