#include "order_endpoint.hpp"

#include "market_types.hpp"
#include "exchange.hpp"

#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>

#include <nlohmann/json.hpp>

#include <stdexcept>

constexpr std::string_view INVALID_INPUT_MESSAGE = "Invalid Input";

constexpr std::string_view INVALID_PRICE_QUANTITY_MESSAGE = "Invalid price or quantity";

constexpr std::string_view MARKET_ID_NOT_FOUND_MESSAGE = "Market ID not found";

constexpr std::string_view ORDER_PLACED_MESSAGE = "Order Placed";

tradesim::order_endpoint::order_endpoint(exchange& e): m_exchange(e) {
}

cppevent::task<void> tradesim::order_endpoint::serve(const cppevent::http_request& req,
                                                     cppevent::http_body& body, 
                                                     cppevent::http_output& res) {
    std::string input;
    co_await body.read(input, LONG_MAX);
    
    order_form form;
    bool invalid_input = false;

    try {
        json j = json::parse(input);
        j.get_to(form);
    } catch (...) {
        invalid_input = true;
    }
    if (invalid_input) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .content_length(INVALID_INPUT_MESSAGE.size())
                    .header("content-type", "text/plain")
                    .write(INVALID_INPUT_MESSAGE);
        co_return;
    }

    if (form.m_price <= 0 || form.m_quantity <= 0) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .content_length(INVALID_PRICE_QUANTITY_MESSAGE.size())
                    .header("content-type", "text/plain")
                    .write(INVALID_PRICE_QUANTITY_MESSAGE);
        co_return;
    }

    bool placed = m_exchange.place_order(form);
    if (placed) {
        co_await res.status(cppevent::HTTP_STATUS::OK)
                    .content_length(ORDER_PLACED_MESSAGE.size())
                    .header("content-type", "text/plain")
                    .write(ORDER_PLACED_MESSAGE);
    } else {
        co_await res.status(cppevent::HTTP_STATUS::NOT_FOUND)
                    .content_length(MARKET_ID_NOT_FOUND_MESSAGE.size())
                    .header("content-type", "text/plain")
                    .write(MARKET_ID_NOT_FOUND_MESSAGE);
    }
}
