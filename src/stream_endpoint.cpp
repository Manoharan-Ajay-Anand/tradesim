#include "stream_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/timer.hpp>

#include <iostream>

using namespace std::chrono_literals;

constexpr std::string_view INVALID_MARKET_ID =
    "status: 400\ncontent-length: 17\ncontent-type: text/plain\n\nInvalid Market ID";

constexpr std::string_view INVALID_TRADER_ID =
    "status: 400\ncontent-length: 17\ncontent-type: text/plain\n\nInvalid Trader ID";


tradesim::stream_endpoint::stream_endpoint(exchange& e, cppevent::event_loop& loop): m_exchange(e),
                                                                                     m_loop(loop) {
}

cppevent::awaitable_task<void> tradesim::stream_endpoint::process(const cppevent::context& cont,
                                                                  cppevent::stream& s_stdin,
                                                                  cppevent::output& o_stdout) {
    auto market_id_str = cont.get_path_segment("marketId").value();
    if (market_id_str.size() > MAX_ID_SIZE) {
        co_await o_stdout.write(INVALID_MARKET_ID);
        co_return;
    }

    auto trader_id_str = cont.get_path_segment("traderId").value();
    if (trader_id_str.size() > MAX_ID_SIZE) {
        co_await o_stdout.write(INVALID_TRADER_ID);
        co_return;
    }

    object_id market_id, trader_id;
    market_id = market_id_str;
    trader_id = trader_id_str;

    co_await o_stdout.write("status: 200\ncontent-type: text/event-stream\nx-accel-buffering: no\n\n");

    market_stream stream { m_loop };

    std::unique_ptr<subscription> sub = m_exchange.subscribe(market_id, trader_id, &stream);
    if (!sub) {
        co_await o_stdout.write("event: duplicate\ndata: {}\n\n");
        co_return;
    }

    while (true) {
        long count = co_await stream.await_items();
        std::string msg;
        for (long i = 0; i < count; ++i) {
            msg.append(stream.front());
            stream.pop();
        }
        co_await o_stdout.write(msg);
    }

}
