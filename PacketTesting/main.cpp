#include <iostream>
#include "serializer.hpp"
#include "packet.hpp"
#include "buffer.hpp"

struct my_special_packet2 {
    uint16_t m_id{};
    uint16_t m_length{};
    std::array<uint64_t, 8ULL> m_data{};

    static void serialize(const my_special_packet2& p, net::ByteBuffer& s) {
        s.put(p.m_id);
        s.put(p.m_length);
        for (const auto& g : p.m_data) {
            s.put(g);
        }
    }

    static void deserialize(my_special_packet2& p, net::ByteBuffer& d) {
        d.get(p.m_id);
        d.get(p.m_length);
        for (int i = 0; i < 8; i++) {
             d.get(p.m_data[i]);
        }
    }
};
enum class PacketIds {
    HELLO
};

struct basic_header {
    uint16_t id{};
    uint16_t size{};
    static void serialize(const basic_header& p, net::ByteBuffer& s) {
        s.put(p.id);
        s.put(p.size);
    }
    static void deserialize(basic_header& p, net::ByteBuffer& d) {
        d.get(p.id);
        d.get(p.size);
    }
};

PacketIds handler(net::ByteBuffer& input) {
    input.flip();
    net::byte_type b_id;
    input.get(b_id);
    input.flip();
    PacketIds id{ b_id };
    return id;
}

int packet_test() {
    using PacketContext = net::PacketManager<PacketIds>;

    PacketContext::init();
    net::Serializer::bind<my_special_packet2>(my_special_packet2::serialize, my_special_packet2::deserialize);
    net::Serializer::bind<basic_header>(basic_header::serialize, basic_header::deserialize);

    PacketContext::registerPacketHandler(handler);
    PacketContext::bind<my_special_packet2>(PacketIds::HELLO);
    net::bytes dataFromSocket = {
        // m_id
        0x00, 0x06,
        // m_length
        0x08, 0x01,
        // m_data
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08 };
    net::ByteBuffer bf{};
    for (auto g : dataFromSocket) {
        bf.put(g);
    }
    // Create our packet
    net::Packet p = PacketContext::create(bf);

    // Serialize it
    net::ByteBuffer ser{};
    net::Serializer::serialize(p, ser);

    // Print serialization
    std::cout << std::hex;
    for (const net::bytes::value_type& c : ser.data()) {
        std::cout << "0x" << +c << " ";
    }
    std::cout << '\n';

    return 0;
}

int buffer_test() {
    net::ByteBuffer buf{};

    buf.put<short>(12).put<long long>(32).put<unsigned int>(2).put<int>(-1231).put<float>(123.123f);
    short a;
    long long b;
    unsigned int c;
    int d;
    float e;
    buf.flip();
    buf.get(a).get(b).get(c).get(d).get(e);

    std::cout << std::dec << "a: " << a << ", b: " << b << ", c: " << c << ", d: " << d << '\n';

    return 0;
}

int main(int argc, char** argv) {
    int ret = 0;
    ret = packet_test();
    ret = buffer_test();

    return ret;
}