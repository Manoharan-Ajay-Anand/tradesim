#include "market.hpp"

tradesim::market::market(cppevent::event_loop& el): m_aq(el), m_task(broadcast_trades()) {
}

cppevent::awaitable_task<void> tradesim::market::broadcast_trades() {
    while (true) {
        long count = co_await m_aq.await_items();
        for (long i = 0; i < count; ++i) {
            m_aq.pop();
        }
    }
}

void tradesim::market::register_account(const object_id& trader_id) {
    m_accounts.try_emplace(trader_id, 0);
}

std::unique_ptr<tradesim::subscription> tradesim::market::subscribe(const object_id& trader_id,
                                                                    cppevent::output* o) {
    auto broadcast_it = m_broadcast.insert(m_broadcast.end(), o);

    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return {};
    }

    auto result = m_stream_map.try_emplace(trader_id, broadcast_it);
    if (!result.second) {
        return {};
    }
    return std::make_unique<tradesim::subscription>(*this, trader_id);
}

void tradesim::market::unsubscribe(const object_id& trader_id) {
    m_stream_map.erase(trader_id);
}
