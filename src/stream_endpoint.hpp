#ifndef TRADESIM_STREAM_ENDPOINT_HPP
#define TRADESIM_STREAM_ENDPOINT_HPP

#include <cppevent_fcgi/endpoint.hpp>

namespace cppevent {

class event_loop;

}

namespace tradesim {

class stream_endpoint : public cppevent::endpoint {
private:
    cppevent::event_loop& m_loop;
    bool m_wait_longer;
public:
    stream_endpoint(cppevent::event_loop& loop);

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
