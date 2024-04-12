#include <iostream>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>
#include <cppevent_fcgi/router.hpp>

#include "exchange.hpp"
#include "welcome_endpoint.hpp"
#include "create_endpoint.hpp"
#include "join_endpoint.hpp"
#include "stream_endpoint.hpp"
#include "order_endpoint.hpp"
#include "cancel_endpoint.hpp"

int main() {
    cppevent::event_loop loop;

    tradesim::exchange ex;

    tradesim::welcome_endpoint welcome;
    tradesim::create_endpoint create { ex };
    tradesim::join_endpoint join { ex };
    tradesim::stream_endpoint stream { ex, loop };
    tradesim::order_endpoint order { ex };
    tradesim::cancel_endpoint cancel { ex };

    cppevent::router routes;
    routes.get("/api/tradesim", welcome);
    routes.post("/api/tradesim/create", create);
    routes.post("/api/tradesim/join", join);
    routes.get("/api/tradesim/stream/{marketId}/{traderId}", stream);
    routes.post("/api/tradesim/order", order);
    routes.post("/api/tradesim/cancel", cancel);

    cppevent::fcgi_server tradesim_server("/tmp/tradesim.sock", loop, routes);

    loop.run();
    return 0;
}
