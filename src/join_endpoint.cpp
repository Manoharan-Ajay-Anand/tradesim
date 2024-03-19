#include "join_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <string>
#include <string_view>
#include <format>

constexpr std::string_view INVALID_INPUT_MESSAGE = 
        "status: 400\ncontent-length: 13\ncontent-type: text/plain\n\nInvalid Input";

constexpr std::string_view MARKET_ID_NOT_EXISTS_MESSAGE = 
        "status: 400\ncontent-length: 24\ncontent-type: text/plain\n\nMarket ID does not exist";

tradesim::join_endpoint::join_endpoint(exchange& e): m_exchange(e) {
}

cppevent::awaitable_task<void> tradesim::join_endpoint::process(const cppevent::context& cont,
                                                                cppevent::stream& s_stdin,
                                                                cppevent::output& o_stdout) {
    long content_length = cont.get_content_len();
    std::string input;
    co_await s_stdin.read(input, content_length, true);
    
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
        co_await o_stdout.write(INVALID_INPUT_MESSAGE);
        co_return;
    }
    
    if (!m_exchange.join_market(market_id, trader_id)) {
        co_await o_stdout.write(MARKET_ID_NOT_EXISTS_MESSAGE);
        co_return;
    }

    std::string response = std::format("content-length: {}\ncontent-type: text/plain\n\n{} {}",
                                        market_id.size() + 1 + trader_id.size(), 
                                        market_id.get_view(), trader_id.get_view());
    co_await o_stdout.write(response);
}

