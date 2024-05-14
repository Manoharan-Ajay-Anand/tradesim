#include "market.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <iostream>

constexpr std::string_view PRICE_POINT_MSG = "pricePoint";

constexpr std::string_view ACCOUNT_MSG = "account";

constexpr std::string_view TRADE_MSG = "trade";

constexpr std::string_view ORDER_SUBMITTED = "orderSubmitted";

constexpr std::string_view ORDER_EXECUTED = "orderExecuted";

constexpr std::string_view ORDER_CANCELLED = "orderCancelled";

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

void tradesim::market::execute_trades() {
    auto bid_it = m_bids.begin();
    auto ask_it = m_asks.begin();
    while (bid_it != m_bids.end() && ask_it != m_asks.end()) {
        if (bid_it->first < ask_it->first) {
            break;
        }

        auto& bid_set = bid_it->second;
        auto& ask_set = ask_it->second;

        auto& bid = m_orders.at(*(bid_set.begin()));
        auto& ask = m_orders.at(*(ask_set.begin()));

        trade t = trade::match(bid, ask);
        update_accounts(bid.m_trader, ask.m_trader, t);

        long quantity_diff = 0 - t.m_quantity;
        update_bid_count(bid.m_price, quantity_diff);
        update_ask_count(ask.m_price, quantity_diff);

        bid.m_quantity -= t.m_quantity;
        order_update bid_ou = { bid.m_id, bid.m_type, t.m_price, t.m_quantity, bid.m_quantity };
        m_broadcast.send_msg(message { bid.m_trader, ORDER_EXECUTED, bid_ou });
        if (bid.m_quantity == 0) {
            m_trader_orders[bid.m_trader].erase(bid.m_id);
            bid_set.erase(bid.m_id);
            m_orders.erase(bid.m_id);
        }

        ask.m_quantity -= t.m_quantity;
        order_update ask_ou = { ask.m_id, ask.m_type, t.m_price, t.m_quantity, ask.m_quantity };
        m_broadcast.send_msg(message { ask.m_trader, ORDER_EXECUTED, ask_ou });
        if (ask.m_quantity == 0) {
            m_trader_orders[ask.m_trader].erase(ask.m_id);
            ask_set.erase(ask.m_id);
            m_orders.erase(ask.m_id);
        }

        if (bid_set.empty()) {
            bid_it = m_bids.erase(bid_it);
        }
        if (ask_set.empty()) {
            ask_it = m_asks.erase(ask_it);
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

        for (long order_id : m_trader_orders[trader_id]) {
            order& o = m_orders.at(order_id);
            order_update ou = { o.m_id, o.m_type, o.m_price, o.m_quantity, o.m_quantity };
            m_broadcast.send_msg(message { trader_id, ORDER_SUBMITTED, ou });
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
    m_bids[o.m_price].insert(o.m_id);
    m_trader_orders[trader_id].insert(o.m_id);

    order_update ou = { o.m_id, o.m_type, o.m_price, o.m_quantity, o.m_quantity };
    update_bid_count(price, quantity);
    m_broadcast.send_msg(message { trader_id, ORDER_SUBMITTED, ou });
    
    execute_trades();
}

void tradesim::market::place_ask(const object_id& trader_id, long price, long quantity) {
    auto it = m_accounts.find(trader_id);
    if (it == m_accounts.end()) {
        return;
    }
    order o = { ++m_order_count, trader_id, order_type::ASK, price, quantity };
    m_orders[o.m_id] = o;
    m_asks[o.m_price].insert(o.m_id);
    m_trader_orders[trader_id].insert(o.m_id);

    order_update ou = { o.m_id, o.m_type, o.m_price, o.m_quantity, o.m_quantity };
    update_ask_count(price, quantity);
    m_broadcast.send_msg(message { trader_id, ORDER_SUBMITTED, ou });

    execute_trades();
}

bool tradesim::market::cancel_order(const object_id& trader_id, long order_id) {
    auto it = m_orders.find(order_id);
    if (it == m_orders.end()) {
        return false;
    }

    order& o = it->second;
    if (o.m_trader != trader_id) {
        return false;
    }
    
    order_update ou = { o.m_id, o.m_type, o.m_price, o.m_quantity, o.m_quantity };
    long diff = 0 - o.m_quantity;
    switch (o.m_type) {
        case order_type::BID: {
            update_bid_count(o.m_price, diff);
            auto& bid_set = m_bids.at(o.m_price);
            bid_set.erase(o.m_id);
            if (bid_set.empty()) {
                m_bids.erase(o.m_price);
            }
            break;
        }
        case order_type::ASK: {
            update_ask_count(o.m_price, diff);
            auto& ask_set = m_asks.at(o.m_price);
            ask_set.erase(o.m_id);
            if (ask_set.empty()) {
                m_asks.erase(o.m_price);
            }
            break;
        }
    }
    m_trader_orders[trader_id].erase(o.m_id);
    m_orders.erase(it);

    m_broadcast.send_msg(message { trader_id, ORDER_CANCELLED, ou });
    return true;
}
