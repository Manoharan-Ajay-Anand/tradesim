#ifndef TRADESIM_CREATE_ENDPOINT_HPP
#define TRADESIM_CREATE_ENDPOINT_HPP

#include <cppevent_fcgi/endpoint.hpp>

namespace tradesim {

class exchange;

class create_endpoint : public cppevent::endpoint {
private:
    exchange& m_exchange;
public:
    create_endpoint(exchange& e);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
