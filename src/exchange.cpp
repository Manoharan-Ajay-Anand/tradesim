#include "exchange.hpp"

bool tradesim::exchange::create_market(std::string_view market_id) {
    return m_markets.try_emplace(std::string { market_id }).second;
}
