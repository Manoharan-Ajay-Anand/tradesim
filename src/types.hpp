#ifndef TRADESIM_TYPES_HPP
#define TRADESIM_TYPES_HPP

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <string_view>

namespace tradesim {

using json = nlohmann::json;

constexpr long MAX_ID_SIZE = 30; 

class object_id {
private:
    std::array<char, MAX_ID_SIZE> m_arr;
    long m_size;

public:
    const object_id& operator=(std::string_view s);

    char* data();

    const char* data() const;

    long size() const;

    void resize(long size);

    std::string_view get_view() const;

    bool operator==(const tradesim::object_id& other) const;
};

void to_json(json& j, const object_id& o);

void from_json(const json& j, object_id& o);

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
