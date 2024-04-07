#include "market.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <iostream>

constexpr std::string_view PRICE_POINT_MSG = "pricePoint";

constexpr std::string_view ACCOUNT_MSG = "account";

constexpr std::string_view TRADE_MSG = "trade";

void tradesim::market::update_bid_count(long price, long diff) {
    auto& price_point = (m_price_points.try_emplace(price, price, 0, 0).first)->second;
    price_point.m_bid_count += diff;
    m_broadcast.send_msg(message { PRICE_POINT_MSG, json { price_point } });
}

void tradesim::market::update_ask_count(long price, long diff) {
    auto& price_point = (m_price_points.try_emplace(price, price, 0, 0).first)->second;
    price_point.m_ask_count += diff;
    m_broadcast.send_msg(message { PRICE_POINT_MSG, json { price_point } });
}

void tradesim::market::update_account(const object_id& trader_id, order_type type, trade t) {
    auto& account = m_accounts.at(trader_id);
    account.confirm(type, t.m_price, t.m_quantity);
    m_broadcast.send_msg(message { trader_id, ACCOUNT_MSG, json { account } });
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

        m_broadcast.send_msg(message { TRADE_MSG, json { t } });

        update_account(bid.m_trader, order_type::BID, t);
        update_account(ask.m_trader, order_type::ASK, t);

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
                                                                    market_stream* m_ptr) {
    if (m_accounts.find(trader_id) == m_accounts.end()) {
        return {};
    }
    return m_broadcast.subscribe(trader_id, m_ptr);
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
