#include <iostream>
#include <format>
#include <string_view>

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

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string_view { argv[1] } != "-p") {
        std::cout << "format: tradesim -p <PORT>" << std::endl;
        return 0;
    }

    char* port = argv[2];

    std::cout << "Starting tradesim server at port " << port << "..." << std::endl;
    cppevent::event_loop loop;

    tradesim::welcome_endpoint welcome;

    cppevent::router routes;
    routes.get("/api", welcome);
    
    cppevent::fcgi_server tradesim_server(NULL, port, loop, routes);

    loop.run();
    return 0;
}
