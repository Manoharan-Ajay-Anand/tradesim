#include "exchange.hpp"

bool tradesim::exchange::create_market(const object_id& market_id) {
    return m_markets.try_emplace(market_id).second;
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
                                                                      market_stream* m_ptr) {
    auto it = m_markets.find(market_id);
    if (it == m_markets.end()) {
        return {};
    }
    return (it->second).subscribe(trader_id, m_ptr);
}

bool tradesim::exchange::place_order(const order_form& form) {
    auto it = m_markets.find(form.m_market_id);
    if (it == m_markets.end()) {
        return false;
    }
    market& m = it->second;
    switch (form.m_type) {
        case order_type::BID:
            m.place_bid(form.m_trader_id, form.m_price, form.m_quantity);
            break;
        case order_type::ASK:
            m.place_ask(form.m_trader_id, form.m_price, form.m_quantity);
            break;
    }
    return true;
}

bool tradesim::exchange::cancel_order(const object_id& market_id,
                                      const object_id& trader_id, long order_id) {
    auto it = m_markets.find(market_id);
    if (it == m_markets.end()) {
        return false;
    }

    return (it->second).cancel_order(trader_id, order_id);
}
