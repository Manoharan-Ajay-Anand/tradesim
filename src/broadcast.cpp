#include "broadcast.hpp"

#include <cppevent_base/status_awaiter.hpp>

#include <cppevent_fcgi/output.hpp>

#include <vector>
#include <format>
#include <iostream>

tradesim::subscription::subscription(broadcast& b, const object_id& id): m_broadcast(b),
                                                                         m_trader_id(id) {
}

tradesim::subscription::~subscription() {
    m_broadcast.unsubscribe(m_trader_id);
}

std::unique_ptr<tradesim::subscription> tradesim::broadcast::subscribe(const object_id& id,
                                                                       market_stream* s_ptr) {
    auto it = m_stream_map.find(id);
    if (it != m_stream_map.end()) {
        return {};
    }
    m_stream_map[id] = m_streams.insert(m_streams.end(), s_ptr);
    return std::make_unique<subscription>(*this, id);
}

void tradesim::broadcast::unsubscribe(const object_id& id) {
    auto it = m_stream_map.find(id);
    if (it != m_stream_map.end()) {
        m_streams.erase(it->second);
        m_stream_map.erase(it);
    }
}

void tradesim::broadcast::send_msg(const message& msg) {
    std::string response = std::format("event: {}\ndata: {}\n\n", msg.m_type, msg.m_content);
    if (msg.m_recipient_opt) {
        auto it = m_stream_map.find(msg.m_recipient_opt.value());
        if (it != m_stream_map.end()) {
            market_stream* m_ptr = *(it->second); 
            m_ptr->push(response);
        }
    } else {
        for (auto m_ptr : m_streams) {
            m_ptr->push(response);
        }
    }
}
