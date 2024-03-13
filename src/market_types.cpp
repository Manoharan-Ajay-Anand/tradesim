#include "market_types.hpp"

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
