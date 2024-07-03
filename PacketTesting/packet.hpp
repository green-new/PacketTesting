#pragma once
#include "def.hpp"
#include "buffer.hpp"

namespace net {

	class Packet {
	public:
		template<class T>
		Packet(T&& object) 
			: m_packet(std::make_shared<PacketModel<T>>(std::forward<T>(object))) { }

		class PacketConcept {
		public:
			virtual ~PacketConcept() = default;
			virtual void serialize(ByteBuffer& in) = 0;
			virtual void deserialize(ByteBuffer& in) = 0;
		};

		template<class T>
		class PacketModel : public PacketConcept {
		public:
			PacketModel(const T& model) 
				: m_object(model) { }
			void serialize(ByteBuffer& out) override {
				net::Serializer::serialize<T>(m_object, out);
			}
			void deserialize(ByteBuffer& out) override {
				net::Serializer::deserialize<T>(m_object, out);
			}
		private:
			T m_object;
		};
	public:
		static void serialize(const Packet& p, ByteBuffer& out) {
			p.m_packet->serialize(out);
		}
		static void deserialize(Packet& p, ByteBuffer& out) {
			p.m_packet->deserialize(out);
		}
	private:
		std::shared_ptr<PacketConcept> m_packet;
	};

	class IPacketFactory {
	public:
		virtual ~IPacketFactory() = default;
		virtual net::Packet create() = 0;
	};

	template<class PacketType>
	class PacketFactory : public IPacketFactory {
	public:
		PacketFactory() { }
	public:
		net::Packet create() override {
			return net::Packet(PacketType());
		}
		//template<class Output, typename... CtorArgs>
		//net::Packet<Output> create(CtorArgs&&... ctorArgs) {
		//	return net::Packet<Output>(PacketType(std::forward<CtorArgs>(ctorArgs)...));
		//}
	};

	// Used to determine packet type at runtime.
	template<class PacketEnums>
	using PacketHandlerFunc = std::function<PacketEnums(ByteBuffer&)>;

	// Packet manager creates and instiantiates new packets based on the provided id class.
	template<class PacketEnums>
	class PacketManager {
	public:
		static void init() {
			// Allow net::Packet to be serialized/deserialized from the underlying type (if its defined).
			net::Serializer::bind<net::Packet>(net::Packet::serialize, net::Packet::deserialize);
		}
		template<class PacketType>
		static void bind(const PacketEnums& id) {
			m_factories.insert({ id, std::make_shared<PacketFactory<PacketType>>() });
		}
		static net::Packet create(net::ByteBuffer& in) {
			PacketEnums userPacketId = m_packetHandler(in);
			net::Packet packet = (m_factories[userPacketId])->create();
			net::Serializer::deserialize(packet, in);
			return packet;
		}
		// Register the user defined packet handler. Runtime function used to determine what packet to create.
		static void registerPacketHandler(const PacketHandlerFunc<PacketEnums>& packetHandler) {
			m_packetHandler = packetHandler;
		}
	private:
		inline static std::unordered_map<PacketEnums, std::shared_ptr<IPacketFactory>> m_factories{};
		inline static PacketHandlerFunc<PacketEnums> m_packetHandler{};
	};
}