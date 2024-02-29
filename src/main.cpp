#include <iostream>
#include <format>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>
#include <cppevent_fcgi/endpoint.hpp>
#include <cppevent_fcgi/router.hpp>

namespace tradesim {

class welcome_endpoint : public cppevent::endpoint {
private:
    std::string response;
public:
    welcome_endpoint() {
        std::string_view welcome_message = "Welcome to TradeSim API";
        response = std::format("content-length: {}\ncontent-type: text/plain\n\n{}",
                                welcome_message.size(), welcome_message);
    }

    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                                 cppevent::stream& s_stdin,
                                                 cppevent::output& o_stdout) {
        co_await o_stdout.write(response);
    }
};

}

int main() {
    std::cout << "Starting tradesim server..." << std::endl;
    cppevent::event_loop loop;

    tradesim::welcome_endpoint welcome;

    cppevent::router routes;
    routes.get("/api", welcome);
    
    cppevent::fcgi_server tradesim_server(NULL, "9000", loop, routes);

    loop.run();
    return 0;
}
