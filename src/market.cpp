#include "market.hpp"

tradesim::market::market(cppevent::event_loop& el): m_aq(el), m_task(broadcast_trades()) {
    m_order_count = 0;
}

cppevent::awaitable_task<void> tradesim::market::broadcast_trades() {
    while (true) {
        long count = co_await m_aq.await_items();
        for (long i = 0; i < count; ++i) {
            m_aq.pop();
        }
    }
}

void tradesim::market::update() {
    while (!m_bids.empty() && !m_asks.empty()) {
        auto& bid = m_bids.top();
        auto& ask = m_asks.top();
        if (bid.m_price < ask.m_price) {
            break;
        }

        long fulfilled = std::min(bid.m_quantity, ask.m_quantity);
        long price = bid.m_price;
        if (ask.m_id < bid.m_id) {
            price = ask.m_price;
        }

        auto& buyer = m_accounts.at(bid.m_trader);
        buyer.confirm(order_type::BID, price, fulfilled);
        auto& seller = m_accounts.at(ask.m_trader);
        seller.confirm(order_type::ASK, price, fulfilled);

        m_price_points[bid.m_price].m_bid_count -= fulfilled;
        m_price_points[ask.m_price].m_ask_count -= fulfilled;

        bid.m_quantity -= fulfilled;
        ask.m_quantity -= fulfilled;
        if (bid.m_quantity == 0) {
            m_bids.pop();
        }
        if (ask.m_quantity == 0) {
            m_asks.pop();
        }
    }
}

void tradesim::market::register_account(const object_id& trader_id) {
    m_accounts.try_emplace(trader_id);
}

std::unique_ptr<tradesim::subscription> tradesim::market::subscribe(const object_id& trader_id,
                                                                    cppevent::output* o) {
    if (m_accounts.find(trader_id) == m_accounts.end()) {
        return {};
    }

    if (m_stream_map.find(trader_id) != m_stream_map.end()) {
        return {};
    }

    auto broadcast_it = m_broadcast.insert(m_broadcast.end(), o);
    m_stream_map[trader_id] = broadcast_it;

    return std::make_unique<tradesim::subscription>(*this, trader_id);
}

void tradesim::market::unsubscribe(const object_id& trader_id) {
    auto s_it = m_stream_map.find(trader_id);
    if (s_it == m_stream_map.end()) {
        return;
    }
    m_broadcast.erase(s_it->second);
    m_stream_map.erase(s_it);
}

void tradesim::market::place_bid(const object_id& trader_id, long price, long quantity) {
    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return;
    }
    m_bids.push({ ++m_order_count, trader_id, order_type::BID, price, quantity });
    m_price_points[price].m_bid_count += quantity;
    update();
}

void tradesim::market::place_ask(const object_id& trader_id, long price, long quantity) {
    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return;
    }
    m_asks.push({ ++m_order_count, trader_id, order_type::ASK, price, quantity });
    m_price_points[price].m_ask_count += quantity;
    update();
}
