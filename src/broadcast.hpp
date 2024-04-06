#ifndef TRADESIM_BROADCAST_HPP
#define TRADESIM_BROADCAST_HPP

#include "types.hpp"

#include <cppevent_base/async_queue.hpp>

#include <unordered_map>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace tradesim {

class broadcast;

class subscription {
private:
    broadcast& m_broadcast;
    object_id m_trader_id;

public:
    subscription(broadcast& b, const object_id& id);
    ~subscription();

    subscription(const subscription&) = delete;
    subscription& operator=(const subscription&) = delete;

    subscription(subscription&&) = delete;
    subscription& operator=(subscription&&) = delete;
};

struct message {
    std::optional<object_id> m_recipient_opt;
    std::string_view m_type;
    std::string m_content;

    message(const object_id& id, std::string_view type, const json& j);

    message(std::string_view type, const json& j);
};

class broadcast {
private:
    std::list<market_stream*> m_streams;
    std::unordered_map<object_id, std::list<market_stream*>::iterator> m_stream_map;
public:
    std::unique_ptr<subscription> subscribe(const object_id& id, market_stream* s_ptr);
    void unsubscribe(const object_id& id);

    void send_msg(const message& msg);
};

}

#endif
