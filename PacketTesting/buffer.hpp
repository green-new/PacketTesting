#pragma once

#include "def.hpp"

namespace net {

	/**
	* Stores bytes as a buffer that can be inserted into or extracted from.
	*/
	class byte_buffer {
	/* Class methods. */
	public:
		/**
		* Default ctor.
		*/
		byte_buffer()
			: m_data(), m_cursor() { }
	/* Modifiers. */
	public:		
		/**
		* Puts a POD type into the byte buffer based on its byte representation.
		* 
		* @tparam PODtype The type of the POD to put.
		*
		* @param pod The pod to put.
		*
		* @returns This @c byte_buffer instance.
		*/
		template<typename PODtype>
		byte_buffer& put(const PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a POD (Plain Old Data) type.");
			const size_t length = sizeof(pod);
			const net::byte_type* bytes = reinterpret_cast<const net::byte_type*>(std::addressof(pod));
			for (uint8_t i = 0; i < length; i++) {
				m_data.push_back(bytes[i]);
			}
			return *this;
		}
		/**
		* Gets a POD type from the byte buffer based on its byte representation.
		* 
		* @tparam PODtype The type of the POD to get.
		*
		* @param pod The pod to overwrite with the byte information.
		*
		* @returns This @c byte_buffer instance.
		*/
		template<typename PODtype>
		byte_buffer& get(PODtype& pod) {
			static_assert(std::is_trivially_copyable<PODtype>::value, "Serialization type must be a POD (Plain Old Data) type.");
			const size_t length = sizeof(pod);
			std::array<net::byte_type, length> temp{};
			for (uint8_t i = 0; i < length; i++) {
				temp[i] = m_data[m_cursor++];
			}
			std::copy(temp.begin(), temp.end(), reinterpret_cast<net::byte_type*>(std::addressof(pod)));
			return *this;
		}
		/**
		* Flips this byte_buffer back to the start.
		*/
		void flip();
		/**
		* Flips the buffer, and clears the buffer.
		*/
		void clear();
	/* Capacity. */
	public:
		/**
		* Size of the buffer. C qualified.
		*
		* @returns Size of the buffer.
		*/
		size_t size() const;
		/**
		* Empty state of the buffer. C qualified.
		*
		* @returns Whether this buffer is empty or nonempty.
		*/
		bool empty() const;
	/* Access. */
	public:
		/**
		* Runs a user provided UnaryFunc algorithm for each element in the buffer.
		* This function is marked constexpr.
		*
		* @tparam UnaryFunc The type that contains the function. Must overload operator(T&).
		* @tparam ExecutionPolicy The standard library execution policy.
		*
		* @param f The algorithm to use.
		* @param policy The execution to policy to use. Default is std::execution::sequenced_policy.
		*/
		template<class UnaryFunc>
		constexpr UnaryFunc for_each(UnaryFunc f) {
			std::for_each(m_data.cbegin(), m_data.cend(), f);
			return f;
		}
		/**
		* Get a const reference to the internal byte buffer. C qualfied, noexcept.
		*/
		const bytes& array() const noexcept;
		/**
		* Get the pointer to the first element of the buffer. C qualified, noexcept.
		*/
		const byte_type* data() const noexcept;
	private:
		/**
		* The internal byte data.
		*/
		bytes m_data{};
		/**
		* The position in the buffer for read and write operations.
		*/
		size_t m_cursor{};
	};
}