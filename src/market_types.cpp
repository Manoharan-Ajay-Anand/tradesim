#include "market_types.hpp"

#include <nlohmann/json.hpp>

void tradesim::from_json(const json& j, order_form& o) {
    j.at("marketId").get_to(o.m_market_id);
    j.at("traderId").get_to(o.m_trader_id);
    auto& type = j.at("type").get_ref<const std::string&>();
    if (type == "bid") {
        o.m_type = order_type::BID;
    } else if (type == "ask") {
        o.m_type = order_type::ASK;
    } else {
        throw std::runtime_error("Invalid type of order");
    }
    j.at("price").get_to(o.m_price);
    j.at("quantity").get_to(o.m_quantity);
}

void tradesim::to_json(json& j, const price_point& pp) {
    j = json { { "price", pp.m_price }, { "bids", pp.m_bid_count }, { "asks", pp.m_ask_count } };
}

void tradesim::position::confirm(order_type type, long price, long quantity) {
    m_count += static_cast<long>(type) * quantity;
    m_pnl -= static_cast<long>(type) * price * quantity;
}

void tradesim::to_json(json& j, const position& pos) {
    j = json { { "pnl", pos.m_pnl }, { "count", pos.m_count } };
}

tradesim::trade tradesim::trade::match(const order& bid, const order& ask) {
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

void tradesim::to_json(json& j, const trade& t) {
    j = json { { "price", t.m_price }, { "quantity", t.m_quantity } };
}
