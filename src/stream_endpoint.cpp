#include "stream_endpoint.hpp"

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/timer.hpp>

#include <iostream>

using namespace std::chrono_literals;

struct dummy {
    dummy() {
        std::cout << "dummy created" << std::endl;
    }

    ~dummy() {
        std::cout << "dummy destructed" << std::endl;
    }
};

tradesim::stream_endpoint::stream_endpoint(cppevent::event_loop& loop): m_loop(loop) {
    m_wait_longer = false;
}

cppevent::awaitable_task<void> tradesim::stream_endpoint::process(const cppevent::context& cont,
                                                                  cppevent::stream& s_stdin,
                                                                  cppevent::output& o_stdout) {
    dummy d;
    co_await o_stdout.write("content-type:text/plain\n\nhello there");

    std::chrono::seconds wait_time = 5s;
    if (m_wait_longer) {
        wait_time = 60s;
    }
    m_wait_longer = !m_wait_longer;
    
    cppevent::timer t { wait_time, m_loop };
    co_await t.wait();

    std::cout << "timer ended" << std::endl;
}
