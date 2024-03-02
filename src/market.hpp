#ifndef TRADESIM_MARKET_HPP
#define TRADESIM_MARKET_HPP

#include "types.hpp"
#include "subscription.hpp"

#include <cppevent_base/task.hpp>
#include <cppevent_base/async_queue.hpp>

#include <string>
#include <string_view>
#include <unordered_map>
#include <list>
#include <memory>

namespace cppevent {
class event_loop;

class output;
}

namespace tradesim {

using broadcast = std::list<cppevent::output*>;

class market {
private:
    std::unordered_map<object_id, long> m_accounts; 
    std::unordered_map<object_id, broadcast::const_iterator> m_stream_map;
    broadcast m_broadcast;
    cppevent::async_queue<long> m_aq;
    cppevent::awaitable_task<void> m_task;

    cppevent::awaitable_task<void> broadcast_trades();
public:
    market(cppevent::event_loop& el);

    market(const market&) = delete;
    market& operator=(const market&) = delete;

    market(market&&) = delete;
    market& operator=(market&&) = delete;

    void register_account(const object_id& trader_id);

    std::unique_ptr<subscription> subscribe(const object_id& trader_id, cppevent::output* o);
    void unsubscribe(const object_id& trader_id);
};

}

#endif
