#ifndef TRADESIM_ORDER_ENDPOINT_HPP
#define TRADESIM_ORDER_ENDPOINT_HPP

#include <cppevent_http/http_endpoint.hpp>

namespace tradesim {

class exchange;

class order_endpoint : public cppevent::http_endpoint {
private:
    exchange& m_exchange;
public:
    order_endpoint(exchange& e);

    cppevent::task<void> serve(const cppevent::http_request& req,
                                cppevent::http_body& body, 
                                cppevent::http_output& res);
};

}

#endif