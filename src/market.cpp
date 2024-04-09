#include "market.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <iostream>

constexpr std::string_view PRICE_POINT_MSG = "pricePoint";

constexpr std::string_view ACCOUNT_MSG = "account";

constexpr std::string_view TRADE_MSG = "trade";

void tradesim::market::update_bid_ask_count(long price, long bid_diff, long ask_diff) {
    auto it = m_price_points.try_emplace(price, price, 0, 0).first;
    
    auto& price_point = it->second;
    price_point.m_bid_count += bid_diff;
    price_point.m_ask_count += ask_diff;
    
    m_broadcast.send_msg(message { PRICE_POINT_MSG, price_point });
    if (price_point.m_bid_count == 0 && price_point.m_ask_count == 0) {
        m_price_points.erase(it);
    }
}

void tradesim::market::update_bid_count(long price, long diff) {
    update_bid_ask_count(price, diff, 0);
}

void tradesim::market::update_ask_count(long price, long diff) {
    update_bid_ask_count(price, 0, diff);
}

void tradesim::market::update_accounts(const object_id& buyer_id, const object_id& seller_id, trade t) {
    m_last_trade = t;
    
    auto& buyer = m_accounts.at(buyer_id);
    buyer.confirm(order_type::BID, t.m_price, t.m_quantity);

    auto& seller = m_accounts.at(seller_id);
    seller.confirm(order_type::ASK, t.m_price, t.m_quantity);
    
    m_broadcast.send_msg(message { TRADE_MSG, t });
    m_broadcast.send_msg(message { buyer_id, ACCOUNT_MSG, buyer });
    m_broadcast.send_msg(message { seller_id, ACCOUNT_MSG, seller });
}

tradesim::bid_ask_pair tradesim::market::get_next_bid_ask_pair() {
    auto bid_it = m_orders.end();
    while (!m_bids.empty()) {
        bid_it = m_orders.find(m_bids.top().m_id);
        if (bid_it != m_orders.end()) {
            break;
        }
        m_bids.pop();
    }

    auto ask_it = m_orders.end();
    while (!m_asks.empty()) {
        ask_it = m_orders.find(m_asks.top().m_id);
        if (ask_it != m_orders.end()) {
            break;
        }
        m_asks.pop();
    }

    return { bid_it != m_orders.end() && ask_it != m_orders.end(), bid_it, ask_it };
}

void tradesim::market::execute_trades() {
    for (bid_ask_pair p = get_next_bid_ask_pair(); p.m_valid; p = get_next_bid_ask_pair()) {
        auto bid_it = p.m_bid_it;
        auto ask_it = p.m_ask_it;
        
        auto& bid = bid_it->second;
        auto& ask = ask_it->second;

        trade t = trade::match(bid, ask);
        if (!t.m_matched) {
            break;
        }

        update_accounts(bid.m_trader, ask.m_trader, t);

        long quantity_diff = 0 - t.m_quantity;
        update_bid_count(bid.m_price, quantity_diff);
        update_ask_count(ask.m_price, quantity_diff);

        bid.m_quantity -= t.m_quantity;
        if (bid.m_quantity == 0) {
            m_orders.erase(bid_it);
            m_bids.pop();
        }

        ask.m_quantity -= t.m_quantity;
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
    std::unique_ptr<subscription> sub_ptr = m_broadcast.subscribe(trader_id, m_ptr);
    if (sub_ptr) {
        m_broadcast.send_msg(message { trader_id, TRADE_MSG, m_last_trade });

        auto& account = m_accounts.at(trader_id);
        m_broadcast.send_msg(message { trader_id, ACCOUNT_MSG, account });

        for (auto& pp : m_price_points) {
            m_broadcast.send_msg(message { trader_id, PRICE_POINT_MSG, pp.second });
        }
    }
    return sub_ptr;
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
