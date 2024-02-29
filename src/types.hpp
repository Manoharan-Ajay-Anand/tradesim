#ifndef TRADESIM_TYPES_HPP
#define TRADESIM_TYPES_HPP

#include "market.hpp"

#include <unordered_map>
#include <string>
#include <memory>

namespace tradesim {

using market_map = std::unordered_map<std::string, std::unique_ptr<market>>;

}

#endif
