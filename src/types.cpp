#include "types.hpp"

#include <cstring>

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
    return m_size == other.m_size && std::memcmp(data(), other.data(), m_size) == 0;
}
