#ifndef TRADESIM_WELCOME_ENDPOINT_HPP
#define TRADESIM_WELCOME_ENDPOINT_HPP

#include <string>

#include <cppevent_http/http_endpoint.hpp>

namespace tradesim {

class welcome_endpoint : public cppevent::http_endpoint {
public:
    cppevent::task<void> serve(const cppevent::http_request& req,
                                cppevent::http_body& body, 
                                cppevent::http_output& res);
};

}

#endif
