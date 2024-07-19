#include "buffer.hpp"

using namespace net;

void byte_buffer::flip() {
	m_cursor = 0;
}
void byte_buffer::clear() {
	m_cursor = 0;
	m_data.clear();
}
size_t byte_buffer::size() const {
	return m_data.size();
}
bool byte_buffer::empty() const {
	return m_data.empty();
}
const bytes& byte_buffer::data() const noexcept {
	return m_data;
}
const byte_type* byte_buffer::data() const noexcept {
	return m_data.data();
}