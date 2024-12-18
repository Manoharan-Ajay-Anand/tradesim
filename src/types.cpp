#include "types.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <charconv>

const tradesim::object_id& tradesim::object_id::operator=(std::string_view s) {
    m_size = s.size();
    std::memcpy(data(), s.data(), m_size);
    return *this;
}

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

void tradesim::to_json(json& j, const object_id& o) {
    j = std::string { o.get_view() };
}

void tradesim::from_json(const json& j, object_id& o) {
    auto& s = j.get_ref<const std::string&>();
    if (s.size() > MAX_ID_SIZE) {
        throw std::runtime_error("ID > 30 characters");
    }
    o = std::string_view { s };
}

long tradesim::to_long(std::string_view s) {
    long l;
    auto result = std::from_chars(s.begin(), s.end(), l);
    if (result.ec != std::errc {}) {
        throw std::runtime_error("tradesim to_long: conversion failed");
    }
    return l;
}
