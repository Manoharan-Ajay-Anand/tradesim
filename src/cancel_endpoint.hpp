#ifndef TRADESIM_CANCEL_ENDPOINT_HPP
#define TRADESIM_CANCEL_ENDPOINT_HPP

#include <cppevent_fcgi/endpoint.hpp>

namespace tradesim {

class exchange;

class cancel_endpoint : public cppevent::endpoint {
private:
    exchange& m_exchange;
public:
    cancel_endpoint(exchange& ex);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
