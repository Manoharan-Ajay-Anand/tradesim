#include "stream_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>

constexpr std::string_view INVALID_MARKET_ID = "Invalid Market ID";

constexpr std::string_view INVALID_TRADER_ID = "Invalid Trader ID";


tradesim::stream_endpoint::stream_endpoint(exchange& e, cppevent::event_loop& loop): m_exchange(e),
                                                                                     m_loop(loop) {
}

cppevent::task<void> receive_messages(tradesim::market_stream& stream,
                                      cppevent::http_output& res) {
    while (true) {
        long count = co_await stream.await_items();
        std::string msg;
        for (long i = 0; i < count; ++i) {
            msg.append(stream.front());
            stream.pop();
        }
        co_await res.write(msg);
    }
}

cppevent::task<void> tradesim::stream_endpoint::serve(const cppevent::http_request& req,
                                                      cppevent::http_body& body, 
                                                      cppevent::http_output& res) {
    auto market_id_str = req.get_path_param("marketId").value();
    if (market_id_str.size() > MAX_ID_SIZE) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(INVALID_MARKET_ID.size())
                    .write(INVALID_MARKET_ID);
        co_return;
    }

    auto trader_id_str = req.get_path_param("traderId").value();
    if (trader_id_str.size() > MAX_ID_SIZE) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(INVALID_TRADER_ID.size())
                    .write(INVALID_TRADER_ID);
        co_return;
    }

    object_id market_id, trader_id;
    market_id = market_id_str;
    trader_id = trader_id_str;

    res.status(cppevent::HTTP_STATUS::OK)
       .header("content-type", "text/event-stream")
       .header("x-accel-buffering","no");

    market_stream stream { m_loop };

    bool subbed = m_exchange.subscribe(market_id, trader_id, &stream);
    if (!subbed) {
        co_await res.write("event: duplicate\ndata: {}\n\n");
        co_return;
    }

    {
        cppevent::task<void> t = receive_messages(stream, res);
        co_await body.await_conn_close();
        m_exchange.unsubscribe(market_id, trader_id);
    }
}
