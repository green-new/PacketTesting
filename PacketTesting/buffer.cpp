#include "buffer.hpp"

using namespace net;

void ByteBuffer::flip() {
	m_cursor = 0;
}
void ByteBuffer::clear() {
	m_cursor = 0;
	m_data.clear();
}
const std::vector<net::byte_type>& ByteBuffer::data() const {
	return m_data;
}