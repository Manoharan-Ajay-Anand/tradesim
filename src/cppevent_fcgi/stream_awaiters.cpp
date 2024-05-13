#include "stream_awaiters.hpp"

#include <cppevent_base/suspended_coro.hpp>

bool cppevent::stream_update_awaiter::await_ready() {
    return m_ended;
}

std::coroutine_handle<> cppevent::stream_update_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_producer.store_handle(handle);
    return m_consumer.retrieve_handle();
}

void cppevent::stream_update_awaiter::await_resume() {
    if (m_ended && m_consumer.has_handle()) {
        m_consumer.retrieve_handle().resume();
    }
}

bool cppevent::stream_readable_awaiter::await_ready() {
    return m_remaining > 0 || m_ended;
}

std::coroutine_handle<> cppevent::stream_readable_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_consumer.store_handle(handle);
    return m_producer.retrieve_handle();
}

bool cppevent::stream_readable_awaiter::await_resume() {
    return m_remaining > 0;
}
