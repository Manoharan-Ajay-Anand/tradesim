#ifndef TRADESIM_TYPES_HPP
#define TRADESIM_TYPES_HPP

#include <array>
#include <algorithm>
#include <string_view>

namespace tradesim {

constexpr long MAX_ID_SIZE = 30; 

constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ull;

constexpr size_t FNV_PRIME = 1099511628211ull;

class object_id {
private:
    std::array<char, MAX_ID_SIZE> m_arr;
    long m_size;

public:
    char* data();

    const char* data() const;

    long size() const;

    void resize(long size);

    std::string_view get_view() const;

    bool operator==(const tradesim::object_id& other) const;
};

}

namespace std {

template <>
struct hash<tradesim::object_id> {
    size_t operator()(const tradesim::object_id& id) const {
        size_t result = tradesim::FNV_OFFSET_BASIS;
        for (long i = 0; i < id.size(); ++i) {
            size_t b = static_cast<size_t>(*(id.data() + i));
            result = (result ^ b) * tradesim::FNV_PRIME;
        }
        return result;
    }
};

}

#endif
