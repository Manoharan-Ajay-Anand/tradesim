#include "market.hpp"

constexpr std::string_view PRICE_POINT_MSG = "pricePoint";

tradesim::market::market(cppevent::event_loop& el): m_messages(el), m_task(broadcast_messages()) {
    m_order_count = 0;
}

cppevent::awaitable_task<void> tradesim::market::broadcast_messages() {
    while ((co_await m_messages.await_items()) > 0) {
        auto& msg = m_messages.front();
        co_await m_broadcast.send_msg(msg);
        m_messages.pop();
    }
}

void tradesim::market::update_bid_count(long price, long diff) {
    auto& price_point = (m_price_points.try_emplace(price, price, 0, 0).first)->second;
    price_point.m_bid_count += diff;
    m_messages.push({ {}, PRICE_POINT_MSG, json { price_point }.dump() });
}

void tradesim::market::update_ask_count(long price, long diff) {
    auto& price_point = (m_price_points.try_emplace(price, price, 0, 0).first)->second;
    price_point.m_ask_count += diff;
    m_messages.push({ {}, PRICE_POINT_MSG, json { price_point }.dump() });
}

namespace tradesim {

struct trade {
    bool m_matched;
    long m_price;
    long m_quantity;

    static trade match(const order& bid, const order& ask) {
        if (bid.m_price < ask.m_price) {
            return { false, 0, 0 };
        }
        long quantity = std::min(bid.m_quantity, ask.m_quantity);
        long price = bid.m_price;
        if (ask.m_id < bid.m_id) {
            price = ask.m_price;
        }
        return { true, price, quantity };
    }
};

}

void tradesim::market::execute_trades() {
    while (!m_bids.empty() && !m_asks.empty()) {
        auto bid_it = m_orders.find(m_bids.top().m_id);
        if (bid_it == m_orders.end()) {
            m_bids.pop();
            continue;
        }

        auto ask_it = m_orders.find(m_asks.top().m_id);
        if (ask_it == m_orders.end()) {
            m_asks.pop();
            continue;
        }
        
        auto& bid = bid_it->second;
        auto& ask = ask_it->second;

        trade t = trade::match(bid, ask);
        if (!t.m_matched) {
            break;
        }

        auto& buyer = m_accounts.at(bid.m_trader);
        buyer.confirm(order_type::BID, t.m_price, t.m_quantity);
        auto& seller = m_accounts.at(ask.m_trader);
        seller.confirm(order_type::ASK, t.m_price, t.m_quantity);

        update_bid_count(bid.m_price, 0 - t.m_quantity);
        update_ask_count(ask.m_price, 0 - t.m_quantity);

        bid.m_quantity -= t.m_quantity;
        ask.m_quantity -= t.m_quantity;
        if (bid.m_quantity == 0) {
            m_orders.erase(bid_it);
            m_bids.pop();
        }
        if (ask.m_quantity == 0) {
            m_orders.erase(ask_it);
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
    return m_broadcast.subscribe(trader_id, o);
}

void tradesim::market::unsubscribe(const object_id& trader_id) {
   m_broadcast.unsubscribe(trader_id);
}

void tradesim::market::place_bid(const object_id& trader_id, long price, long quantity) {
    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return;
    }
    order o = { ++m_order_count, trader_id, order_type::BID, price, quantity };
    m_orders[o.m_id] = o;
    m_bids.push({o.m_id, o.m_price});
    update_bid_count(price, quantity);
    execute_trades();
}

void tradesim::market::place_ask(const object_id& trader_id, long price, long quantity) {
    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return;
    }
    order o = { ++m_order_count, trader_id, order_type::ASK, price, quantity };
    m_orders[o.m_id] = o;
    m_asks.push({o.m_id, o.m_price});
    update_ask_count(price, quantity);
    execute_trades();
}
