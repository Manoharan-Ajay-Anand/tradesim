#ifndef TRADESIM_BROADCAST_HPP
#define TRADESIM_BROADCAST_HPP

#include "types.hpp"

#include <cppevent_base/task.hpp>

#include <unordered_map>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace cppevent {

class output;

}

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
};

class broadcast {
private:
    std::list<cppevent::output*> m_outputs;
    std::unordered_map<object_id, std::list<cppevent::output*>::iterator> m_output_map;
public:
    std::unique_ptr<subscription> subscribe(const object_id& id, cppevent::output* o);
    void unsubscribe(const object_id& id);

    cppevent::awaitable_task<void> send_msg(const message& msg);
};

}

#endif
