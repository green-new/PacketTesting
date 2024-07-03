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

#define ASIO_STANDALONE
#include <asio.hpp>
#include <boost/asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <SDKDDKVer.h>

namespace net {
    constexpr std::size_t max_read = 8U * 1024U;
    constexpr size_t max_bytes = 4U * 1024U;
    using byte_type = boost::asio::detail::buffered_stream_storage::byte_type;
    using bytes = std::vector<byte_type>;
}