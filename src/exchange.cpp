#include "exchange.hpp"

tradesim::exchange::exchange(cppevent::event_loop& loop): m_loop(loop) {
}

bool tradesim::exchange::create_market(const object_id& market_id) {
    return m_markets.try_emplace(market_id, std::ref(m_loop)).second;
}

bool tradesim::exchange::join_market(const object_id& market_id, const object_id& trader_id) {
    auto it = m_markets.find(market_id);
    if (it == m_markets.end()) {
        return false;
    }
    (it->second).register_account(trader_id);
    return true;
}

std::unique_ptr<tradesim::subscription> tradesim::exchange::subscribe(const object_id& market_id,
                                                                      const object_id& trader_id,
                                                                      cppevent::output* o) {
    auto it = m_markets.find(market_id);
    if (it == m_markets.end()) {
        return {};
    }
    return (it->second).subscribe(trader_id, o);
}
