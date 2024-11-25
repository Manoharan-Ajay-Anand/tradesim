#ifndef TRADESIM_CANCEL_ENDPOINT_HPP
#define TRADESIM_CANCEL_ENDPOINT_HPP

#include <cppevent_http/http_endpoint.hpp>

namespace tradesim {

class exchange;

class cancel_endpoint : public cppevent::http_endpoint {
private:
    exchange& m_exchange;
public:
    cancel_endpoint(exchange& ex);

    cppevent::task<void> serve(const cppevent::http_request& req,
                                cppevent::http_body& body, 
                                cppevent::http_output& res);
};

}

#endif
