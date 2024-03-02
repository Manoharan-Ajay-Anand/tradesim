#include "types.hpp"

char* tradesim::object_id::data() {
    return m_arr.data();
}

const char* tradesim::object_id::data() const {
    return m_arr.data();
}

long tradesim::object_id::size() const {
    return m_size;
}

void tradesim::object_id::resize(long size) {
    m_size = std::min(size, MAX_ID_SIZE);
}

std::string_view tradesim::object_id::get_view() const {
    return { m_arr.data(), static_cast<std::size_t>(m_size) };
}

bool tradesim::object_id::operator==(const tradesim::object_id& other) const {
    if (m_size != other.m_size) {
        return false;
    }
    for (long i = 0; i < m_size; ++i) {
        if (m_arr[i] != other.m_arr[i]) {
            return false;
        }
    }
    return true;
}

