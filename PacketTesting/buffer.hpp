#pragma once

#include "def.hpp"

namespace net {

	class ByteBuffer {
	public:
		ByteBuffer()
			: m_data(), m_cursor() { }
	public:		
		template<typename PODtype>
		ByteBuffer& put(const PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a pod type");
			const size_t length = sizeof(pod);
			const net::byte_type* bytes = reinterpret_cast<const net::byte_type*>(std::addressof(pod));
			for (uint8_t i = 0; i < length; i++) {
				m_data.push_back(bytes[i]);
			}
			return *this;
		}
		template<typename PODtype>
		ByteBuffer& get(PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a pod type");
			const size_t length = sizeof(pod);
			std::array<net::byte_type, length> temp{};
			for (uint8_t i = 0; i < length; i++) {
				temp[i] = m_data[m_cursor++];
			}
			std::copy(temp.begin(), temp.end(), reinterpret_cast<net::byte_type*>(std::addressof(pod)));
			return *this;
		}
	public:
		void flip();
		void clear();
	public:
		const std::vector<net::byte_type>& data() const;
	private:
		std::vector<net::byte_type> m_data{};
		size_t m_cursor{};
	};
}