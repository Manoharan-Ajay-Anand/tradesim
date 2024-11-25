#ifndef TRADESIM_STREAM_ENDPOINT_HPP
#define TRADESIM_STREAM_ENDPOINT_HPP

#include <cppevent_http/http_endpoint.hpp>

namespace cppevent {

class event_loop;

}

namespace tradesim {

class exchange;

class stream_endpoint : public cppevent::http_endpoint {
private:
    cppevent::event_loop& m_loop;
    exchange& m_exchange;
public:
    stream_endpoint(exchange& e, cppevent::event_loop& loop);

    cppevent::task<void> serve(const cppevent::http_request& req,
                                cppevent::http_body& body, 
                                cppevent::http_output& res);
};

}

#endif
