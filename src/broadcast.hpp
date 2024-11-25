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
    bool subscribe(const object_id& id, market_stream* s_ptr);
    void unsubscribe(const object_id& id);

    void send_msg(const message& msg);
};

}

#endif
