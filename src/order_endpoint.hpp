#ifndef TRADESIM_ORDER_ENDPOINT_HPP
#define TRADESIM_ORDER_ENDPOINT_HPP

#include <cppevent_fcgi/endpoint.hpp>

namespace tradesim {

class exchange;

class order_endpoint : public cppevent::endpoint {
private:
    exchange& m_exchange;
public:
    order_endpoint(exchange& e);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif