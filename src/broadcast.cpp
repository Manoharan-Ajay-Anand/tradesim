#include "broadcast.hpp"

#include <cppevent_base/status_awaiter.hpp>

#include <cppevent_fcgi/output.hpp>

#include <vector>
#include <format>

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

cppevent::awaitable_task<void> tradesim::broadcast::send_msg(const message& msg) {
    std::string response = std::format("event: {}\ndata: {}\n\n", msg.m_type, msg.m_content);
    if (msg.m_recipient_opt) {
        auto it = m_output_map.find(msg.m_recipient_opt.value());
        if (it != m_output_map.end()) {
            cppevent::output* o_ptr = *(it->second); 
            co_await o_ptr->write(response);
        }
    } else {
        for (auto o_ptr : m_outputs) {
            co_await o_ptr->write(response);
        }
    }
}
