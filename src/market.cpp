#include "market.hpp"

tradesim::market::market() {
    m_count = 0;
}

void tradesim::market::subscribe() {
    ++m_count;
}
