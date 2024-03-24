#include "market_types.hpp"

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

long tradesim::position::get_pnl() const {
    return m_pnl;
}

long tradesim::position::get_count() const {
    return m_count;
}

void tradesim::position::confirm(order_type type, long price, long quantity) {
    m_count += static_cast<long>(type) * quantity;
    m_pnl -= static_cast<long>(type) * price * quantity;
}
