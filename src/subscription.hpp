#ifndef TRADESIM_SUBSCRIPTION_HPP
#define TRADESIM_SUBSCRIPTION_HPP

#include "types.hpp"

namespace tradesim {

class market;

class subscription {
private:
    market& m_market;
    object_id m_trader_id;

public:
    subscription(market& m, const object_id& id);
    ~subscription();

    subscription(const subscription&) = delete;
    subscription& operator=(const subscription&) = delete;

    subscription(subscription&&) = delete;
    subscription& operator=(subscription&&) = delete;
};

}

#endif
