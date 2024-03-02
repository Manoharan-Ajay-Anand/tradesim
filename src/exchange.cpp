#include "exchange.hpp"

tradesim::exchange::exchange(cppevent::event_loop& loop): m_loop(loop) {
}

bool tradesim::exchange::create_market(const object_id& market_id) {
    return m_markets.try_emplace(market_id, std::ref(m_loop)).second;
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
