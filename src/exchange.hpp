#ifndef TRADESIM_EXCHANGE_HPP
#define TRADESIM_EXCHANGE_HPP

#include "market.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace tradesim {

class exchange {
private:
    std::unordered_map<std::string, market> m_markets;

public:
    bool create_market(std::string_view market_id);
};

}

#endif
