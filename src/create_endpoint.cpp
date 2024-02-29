#include "create_endpoint.hpp"

#include <array>
#include <string_view>
#include <format>

constexpr long MAX_MARKET_ID_LENGTH = 20;

constexpr std::string_view MARKET_ID_MAX_LENGTH_MESSAGE = 
        "status: 400\ncontent-length: 31\ncontent-type: text/plain\n\nMax 20 characters for Market ID";

constexpr std::string_view MARKET_ID_EXISTS_MESSAGE = 
        "status: 400\ncontent-length: 24\ncontent-type: text/plain\n\nMarket ID already exists";

tradesim::create_endpoint::create_endpoint(market_map& markets): m_markets(markets) {
}

cppevent::awaitable_task<void> tradesim::create_endpoint::process(const cppevent::context& cont,
                                                                  cppevent::stream& s_stdin,
                                                                  cppevent::output& o_stdout) {
    long content_length = cont.get_content_len();
    if (content_length > MAX_MARKET_ID_LENGTH) {
        co_await o_stdout.write(MARKET_ID_MAX_LENGTH_MESSAGE);
        co_return;
    }

    std::array<char, MAX_MARKET_ID_LENGTH> buffer;
    co_await s_stdin.read(buffer.data(), content_length, true);
    std::string market_id { buffer.data(), static_cast<std::size_t>(content_length) };

    auto it = m_markets.find(market_id);
    if (it != m_markets.end()) {
        co_await o_stdout.write(MARKET_ID_EXISTS_MESSAGE);
        co_return;
    }

    m_markets[market_id] = std::make_unique<market>();
    std::string response = std::format("content-length: {}\ncontent-type: text/plain\n\n{}",
                                        content_length, market_id);
    co_await o_stdout.write(response);
}
