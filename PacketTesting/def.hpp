#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <queue>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <array>
#include <list>
#include <ostream>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <utility>

namespace net {
    constexpr std::size_t max_read = 8U * 1024U;
    constexpr size_t max_bytes = 4U * 1024U;
    using byte_type = uint8_t;
    using bytes = std::vector<byte_type>;
}