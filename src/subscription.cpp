#include "subscription.hpp"

#include "market.hpp"

tradesim::subscription::subscription(market& m, const object_id& id): m_market(m),
                                                                      m_trader_id(id) {
}

tradesim::subscription::~subscription() {
    m_market.unsubscribe(m_trader_id);
}
