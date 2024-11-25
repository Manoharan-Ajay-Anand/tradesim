#include "join_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>
#include <format>

constexpr std::string_view INVALID_INPUT_MESSAGE = "Invalid Input";

constexpr std::string_view MARKET_ID_NOT_EXISTS_MESSAGE = "Market ID does not exist";

tradesim::join_endpoint::join_endpoint(exchange& e): m_exchange(e) {
}

cppevent::task<void> tradesim::join_endpoint::serve(const cppevent::http_request& req,
                                                    cppevent::http_body& body, 
                                                    cppevent::http_output& res) {
    
    std::string input;
    co_await body.read(input, LONG_MAX);
    
    object_id market_id;
    object_id trader_id;
    bool invalid_input = false;
    try {
        json j = json::parse(input);
        market_id = j.at("marketId").get<object_id>();
        trader_id = j.at("traderId").get<object_id>();
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
    
    if (!m_exchange.join_market(market_id, trader_id)) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(MARKET_ID_NOT_EXISTS_MESSAGE.size())
                    .write(MARKET_ID_NOT_EXISTS_MESSAGE);
        co_return;
    }

    std::string response = std::format("{} {}", market_id.get_view(), trader_id.get_view());
    co_await res.status(cppevent::HTTP_STATUS::OK)
                .header("content-type", "text/plain")
                .content_length(response.size())
                .write(response);
}

