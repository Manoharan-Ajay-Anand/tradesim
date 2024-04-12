#include "cancel_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <nlohmann/json.hpp>

#include <string_view>
#include <format>

constexpr std::string_view INVALID_INPUT_MESSAGE = 
        "status: 400\ncontent-length: 13\ncontent-type: text/plain\n\nInvalid input";

constexpr std::string_view ORDER_NOT_FOUND_MESSAGE = 
        "status: 404\ncontent-length: 15\ncontent-type: text/plain\n\nOrder not found";

constexpr std::string_view ORDER_CANCELLED_MESSAGE = 
        "status: 200\ncontent-length: 15\ncontent-type: text/plain\n\nOrder cancelled";

tradesim::cancel_endpoint::cancel_endpoint(exchange& e): m_exchange(e) {
}

cppevent::awaitable_task<void> tradesim::cancel_endpoint::process(const cppevent::context& cont,
                                                                  cppevent::stream& s_stdin,
                                                                  cppevent::output& o_stdout) {
    long content_length = cont.get_content_len();
    std::string input;
    co_await s_stdin.read(input, content_length, true);
    
    object_id market_id;
    object_id trader_id;
    long order_id;
    bool invalid_input = false;
    try {
        json j = json::parse(input);
        market_id = j.at("marketId").get<object_id>();
        trader_id = j.at("traderId").get<object_id>();
        order_id = j.at("orderId").get<long>();
    } catch (...) {
        invalid_input = true;
    }
    if (invalid_input) {
        co_await o_stdout.write(INVALID_INPUT_MESSAGE);
        co_return;
    }

    if (m_exchange.cancel_order(market_id, trader_id, order_id)) {
        co_await o_stdout.write(ORDER_CANCELLED_MESSAGE);
    } else {
        co_await o_stdout.write(ORDER_NOT_FOUND_MESSAGE);
    }
}
