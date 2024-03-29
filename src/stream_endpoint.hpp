#ifndef TRADESIM_STREAM_ENDPOINT_HPP
#define TRADESIM_STREAM_ENDPOINT_HPP

#include <cppevent_fcgi/endpoint.hpp>

namespace cppevent {

class event_loop;

}

namespace tradesim {

class exchange;

class stream_endpoint : public cppevent::endpoint {
private:
    cppevent::event_loop& m_loop;
    exchange& m_exchange;
public:
    stream_endpoint(exchange& e, cppevent::event_loop& loop);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
