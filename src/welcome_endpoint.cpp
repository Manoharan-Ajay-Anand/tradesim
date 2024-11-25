#include "welcome_endpoint.hpp"

#include <cppevent_http/http_output.hpp>

#include <string_view>
#include <format>

constexpr std::string_view response = "Welcome to TradeSim API";

cppevent::task<void> tradesim::welcome_endpoint::serve(const cppevent::http_request& req,
                                                       cppevent::http_body& body, 
                                                       cppevent::http_output& res) {
    co_await res.status(cppevent::HTTP_STATUS::OK)
                .content_length(response.size())
                .header("content-type", "text/plain")
                .write(response);
}
