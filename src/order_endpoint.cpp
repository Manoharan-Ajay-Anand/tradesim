#include "order_endpoint.hpp"

#include "market_types.hpp"
#include "exchange.hpp"

#include <stdexcept>

constexpr std::string_view INVALID_INPUT_MESSAGE = 
        "status: 400\ncontent-length: 13\ncontent-type: text/plain\n\nInvalid Input";


constexpr std::string_view MARKET_ID_NOT_FOUND_MESSAGE = 
        "status: 404\ncontent-length: 19\ncontent-type: text/plain\n\nMarket ID not found";

constexpr std::string_view ORDER_PLACED_MESSAGE = 
        "status: 200\ncontent-length: 12\ncontent-type: text/plain\n\nOrder Placed";

tradesim::order_endpoint::order_endpoint(exchange& e): m_exchange(e) {
}

cppevent::awaitable_task<void> tradesim::order_endpoint::process(const cppevent::context& cont,
                                                                 cppevent::stream& s_stdin,
                                                                 cppevent::output& o_stdout) {
    long content_length = cont.get_content_len();
    std::string input;
    co_await s_stdin.read(input, content_length, true);
    
    order_form form;
    bool invalid_input = false;

    try {
        json j = json::parse(input);
        j.get_to(form);
    } catch (...) {
        invalid_input = true;
    }
    if (invalid_input) {
        co_await o_stdout.write(INVALID_INPUT_MESSAGE);
        co_return;
    }

    bool placed = m_exchange.place_order(form);
    if (placed) {
        co_await o_stdout.write(ORDER_PLACED_MESSAGE);
    } else {
        co_await o_stdout.write(MARKET_ID_NOT_FOUND_MESSAGE);
    }
}
