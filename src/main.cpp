#include <iostream>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>
#include <cppevent_fcgi/router.hpp>

#include "exchange.hpp"
#include "welcome_endpoint.hpp"
#include "create_endpoint.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string_view { argv[1] } != "-p") {
        std::cout << "format: tradesim -p <PORT>" << std::endl;
        return 1;
    }

    char* port = argv[2];

    std::cout << "Starting tradesim server at port " << port << "..." << std::endl;
    cppevent::event_loop loop;

    tradesim::exchange ex;

    tradesim::welcome_endpoint welcome;
    tradesim::create_endpoint create { ex };

    cppevent::router routes;
    routes.get("/api", welcome);
    routes.post("/api/create", create);
    
    cppevent::fcgi_server tradesim_server(NULL, port, loop, routes);

    loop.run();
    return 0;
}
