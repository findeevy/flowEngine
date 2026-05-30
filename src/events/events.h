#pragma once
#include <variant>

struct EvSelectServerMode {};
struct EvSelectClientMode {};

using Event = std::variant<EvSelectServerMode, EvSelectClientMode>;
