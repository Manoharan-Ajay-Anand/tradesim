#include "cancel_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>

#include <nlohmann/json.hpp>

#include <string_view>
#include <format>

constexpr std::string_view INVALID_INPUT_MESSAGE = "Invalid input";

constexpr std::string_view ORDER_NOT_FOUND_MESSAGE = "Order not found";

constexpr std::string_view ORDER_CANCELLED_MESSAGE = "Order cancelled";

tradesim::cancel_endpoint::cancel_endpoint(exchange& e): m_exchange(e) {
}

cppevent::task<void> tradesim::cancel_endpoint::serve(const cppevent::http_request& req,
                                                      cppevent::http_body& body, 
                                                      cppevent::http_output& res) {
    std::string input;
    co_await body.read(input, LONG_MAX);
    
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
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(INVALID_INPUT_MESSAGE.size())
                    .write(INVALID_INPUT_MESSAGE);
        co_return;
    }

    if (m_exchange.cancel_order(market_id, trader_id, order_id)) {
        co_await res.status(cppevent::HTTP_STATUS::OK)
                    .header("content-type", "text/plain")
                    .content_length(ORDER_CANCELLED_MESSAGE.size())
                    .write(ORDER_CANCELLED_MESSAGE);
    } else {
        co_await res.status(cppevent::HTTP_STATUS::NOT_FOUND)
                    .header("content-type", "text/plain")
                    .content_length(ORDER_NOT_FOUND_MESSAGE.size())
                    .write(ORDER_NOT_FOUND_MESSAGE);
    }
}
