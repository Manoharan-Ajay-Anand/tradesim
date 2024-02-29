#ifndef TRADESIM_CREATE_ENDPOINT_HPP
#define TRADESIM_CREATE_ENDPOINT_HPP

#include "types.hpp"

#include <cppevent_fcgi/endpoint.hpp>

namespace tradesim {

class create_endpoint : public cppevent::endpoint {
private:
    market_map& m_markets;
public:
    create_endpoint(market_map& markets);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
