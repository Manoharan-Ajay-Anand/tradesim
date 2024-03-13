#ifndef TRADESIM_TYPES_HPP
#define TRADESIM_TYPES_HPP

#include <array>
#include <algorithm>
#include <string_view>

namespace tradesim {

constexpr long MAX_ID_SIZE = 30; 

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
        return hash<string_view>{}(id.get_view());
    }
};

}

#endif
