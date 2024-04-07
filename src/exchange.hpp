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
    std::unordered_map<object_id, market> m_markets;
public:
    bool create_market(const object_id& market_id);

    bool join_market(const object_id& market_id, const object_id& trader_id);

    std::unique_ptr<subscription> subscribe(const object_id& market_id,
                                            const object_id& trader_id,
                                            market_stream* m_ptr);
    
    bool place_order(const order_form& form);
};

}

#endif
