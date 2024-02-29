#include "welcome_endpoint.hpp"

#include <string_view>
#include <format>

constexpr std::string_view response = 
        "content-length: 23\ncontent-type: text/plain\n\nWelcome to TradeSim API";

cppevent::awaitable_task<void> tradesim::welcome_endpoint::process(const cppevent::context& cont,
                                                                   cppevent::stream& s_stdin,
                                                                   cppevent::output& o_stdout) {
    co_await o_stdout.write(response);
}
