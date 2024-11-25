#include "create_endpoint.hpp"

#include "types.hpp"
#include "exchange.hpp"

#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>

#include <string_view>
#include <format>

constexpr std::string_view MARKET_ID_MAX_LENGTH_MESSAGE = "Max 30 characters for Market ID";

constexpr std::string_view MARKET_ID_EXISTS_MESSAGE = "Market ID already exists";

tradesim::create_endpoint::create_endpoint(exchange& e): m_exchange(e) {
}

cppevent::task<void> tradesim::create_endpoint::serve(const cppevent::http_request& req,
                                                      cppevent::http_body& body, 
                                                      cppevent::http_output& res) {

    long content_length = to_long(req.get_header("content_length").value_or("0"));
    if (content_length > MAX_ID_SIZE) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(MARKET_ID_MAX_LENGTH_MESSAGE.size())
                    .write(MARKET_ID_MAX_LENGTH_MESSAGE);
        co_return;
    }

    object_id market_id;
    co_await body.read(market_id.data(), content_length);
    market_id.resize(content_length);

    if (!m_exchange.create_market(market_id)) {
        co_await res.status(cppevent::HTTP_STATUS::BAD_REQUEST)
                    .header("content-type", "text/plain")
                    .content_length(MARKET_ID_EXISTS_MESSAGE.size())
                    .write(MARKET_ID_EXISTS_MESSAGE);
        co_return;
    }

    co_await res.status(cppevent::HTTP_STATUS::OK)
                .header("content-type", "text/plain")
                .content_length(market_id.size())
                .write(market_id.get_view());
}
