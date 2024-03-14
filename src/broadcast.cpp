#include "broadcast.hpp"

tradesim::subscription::subscription(broadcast& b, const object_id& id): m_broadcast(b),
                                                                         m_trader_id(id) {
}

tradesim::subscription::~subscription() {
    m_broadcast.unsubscribe(m_trader_id);
}

std::unique_ptr<tradesim::subscription> tradesim::broadcast::subscribe(const object_id& id,
                                                                       cppevent::output* o) {
    auto it = m_output_map.find(id);
    if (it != m_output_map.end()) {
        return {};
    }
    m_output_map[id] = m_outputs.insert(m_outputs.end(), o);
    return std::make_unique<subscription>(*this, id);
}

void tradesim::broadcast::unsubscribe(const object_id& id) {
    auto it = m_output_map.find(id);
    if (it != m_output_map.end()) {
        m_outputs.erase(it->second);
        m_output_map.erase(it);
    }
}
