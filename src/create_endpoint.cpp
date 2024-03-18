#include "create_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <string_view>
#include <format>

constexpr std::string_view MARKET_ID_MAX_LENGTH_MESSAGE = 
        "status: 400\ncontent-length: 31\ncontent-type: text/plain\n\nMax 30 characters for Market ID";

constexpr std::string_view MARKET_ID_EXISTS_MESSAGE = 
        "status: 400\ncontent-length: 24\ncontent-type: text/plain\n\nMarket ID already exists";

tradesim::create_endpoint::create_endpoint(exchange& e): m_exchange(e) {
}

cppevent::awaitable_task<void> tradesim::create_endpoint::process(const cppevent::context& cont,
                                                                  cppevent::stream& s_stdin,
                                                                  cppevent::output& o_stdout) {
    long content_length = cont.get_content_len();
    if (content_length > MAX_ID_SIZE) {
        co_await o_stdout.write(MARKET_ID_MAX_LENGTH_MESSAGE);
        co_return;
    }

    object_id market_id;
    co_await s_stdin.read(market_id.data(), content_length, true);
    market_id.resize(content_length);

    if (!m_exchange.create_market(market_id)) {
        co_await o_stdout.write(MARKET_ID_EXISTS_MESSAGE);
        co_return;
    }

    std::string response = std::format("content-length: {}\ncontent-type: text/plain\n\n{}",
                                        content_length, market_id.get_view());
    co_await o_stdout.write(response);
}
