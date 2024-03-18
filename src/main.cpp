#include <iostream>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>
#include <cppevent_fcgi/router.hpp>

#include "exchange.hpp"
#include "welcome_endpoint.hpp"
#include "create_endpoint.hpp"
#include "join_endpoint.hpp"
#include "stream_endpoint.hpp"

int main() {
    cppevent::event_loop loop;

    tradesim::exchange ex { loop };

    tradesim::welcome_endpoint welcome;
    tradesim::create_endpoint create { ex };
    tradesim::join_endpoint join { ex };
    tradesim::stream_endpoint stream { loop };

    cppevent::router routes;
    routes.get("/api", welcome);
    routes.post("/api/create", create);
    routes.post("/api/join", join);
    routes.get("/api/stream", stream);
    
    cppevent::fcgi_server tradesim_server("/tmp/tradesim.sock", loop, routes);

    loop.run();
    return 0;
}
