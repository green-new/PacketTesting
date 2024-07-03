#pragma once

#include "def.hpp"

namespace net {

	enum class ByteOrder {
		BIG_ENDIAN,
		LITTLE_ENDIAN
	};

	class ByteBuffer {
	public:
		ByteBuffer()
			: m_data(), m_cursor() { }
	public:		
		template<typename PODtype>
		ByteBuffer& put(const PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a pod type");
			size_t bytes = sizeof(pod);
			net::byte_type* pod_to_bytes = reinterpret_cast<net::byte_type*>(std::addressof(pod));
			if (s_order == ByteOrder::BIG_ENDIAN) {
				for (size_t i = bytes; i != 0; ) {
					m_data.push_back(static_cast<net::byte_type>(*(pod_to_bytes + --i)));
				}
			} else if (s_order == ByteOrder::LITTLE_ENDIAN) {
				for (size_t i = 0; i != bytes + 1; ) {
					m_data.push_back(static_cast<net::byte_type>(*(pod_to_bytes + i++)));
				}
			}
			return *this;
		}
		template<typename PODtype>
		ByteBuffer& get(PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a pod type");
			pod = 0;
			size_t bytes = sizeof(pod);
			PODtype* bytes_to_pod = reinterpret_cast<PODtype*>(std::addressof(pod));
			if (s_order == ByteOrder::BIG_ENDIAN) {
				for (size_t i = bytes; i != 0; ) {
					pod |= static_cast<PODtype>((m_data[m_cursor++] << (--i * 8)));
				}
			} else if (s_order == ByteOrder::LITTLE_ENDIAN) {
				for (size_t i = 0; i != bytes + 1; ) {
					pod |= static_cast<PODtype>((m_data[m_cursor++] << (i++ * 8)));
				}
			}
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
		static inline ByteOrder s_order = ByteOrder::BIG_ENDIAN;
	};
}