#ifndef TRADESIM_EXCHANGE_HPP
#define TRADESIM_EXCHANGE_HPP

#include "types.hpp"
#include "market.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace tradesim {

class exchange {
private:
    cppevent::event_loop& m_loop;
    std::unordered_map<object_id, market> m_markets;
public:
    exchange(cppevent::event_loop& loop);

    bool create_market(const object_id& market_id);

    std::unique_ptr<subscription> subscribe(const object_id& market_id,
                                            const object_id& trader_id,
                                            cppevent::output* o);
};

}

#endif
