#ifndef TRADESIM_WELCOME_ENDPOINT_HPP
#define TRADESIM_WELCOME_ENDPOINT_HPP

#include <string>

#include <cppevent_fcgi/endpoint.hpp>

namespace tradesim {

class welcome_endpoint : public cppevent::endpoint {
public:
    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout);
};

}

#endif
