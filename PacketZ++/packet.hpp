#pragma once
#include "def.hpp"
#include "buffer.hpp"

namespace net {

	/**
	* Type erased packet type.
	*/
	class Packet {
	public:
		/**
		* Instantiates an empty packet with no type.
		*/
		Packet() 
			: m_packet() { }
		/**
		* Instantiates a type erased T.
		* @tparam T The type to erase.
		*/
		template<class T>
		Packet(T&& object) 
			: m_packet(std::make_shared<PacketModel<T>>(std::forward<T>(object))) { }
		
		/**
		* Base interface for erased types.
		*/
		class PacketConcept {
		public:
			virtual ~PacketConcept() = default;
			/**
			* Serializes this packet (packet -> raw data) based on its type and the user defined serialization method.
			*/
			virtual void serialize(byte_buffer& out) = 0;
			/**
			* Deserializes this packet (raw data -> packet) based on its type and the user defined deserialization method.
			*/
			virtual void deserialize(byte_buffer& in) = 0;
		};

		/**
		* Concrete type encapulsating the type to erase.
		* @extends PacketConcept
		* @tparam T The user defined packet type that will be erased.
		*/
		template<class T>
		class PacketModel : public PacketConcept {
		public:
			/**
			* Ctor copying the packet. 
			*/
			PacketModel(const T& model) 
				: m_object(model) { }
			/**
			* Calls the type's serialization method from serializer.
			*/
			void serialize(byte_buffer& out) override {
				// We are now tightly coupled with 'serializer', which could be better I think...
				net::serializer::serialize<T>(m_object, out);
			}
			/**
			* Calls the type's deserialization method from serializer.
			*/
			void deserialize(byte_buffer& in) override {
				net::serializer::deserialize<T>(m_object, in);
			}
		private:
			/**
			* The user defined packet.
			*/
			T m_object;
		};
	/** 
	* Modifiers
	*/
	public:
		static void serialize(const Packet& in, byte_buffer& out) {
			in.m_packet->serialize(out);
		}
		static void deserialize(Packet& out, byte_buffer& in) {
			out.m_packet->deserialize(in);
		}
	/** Capacity */
	public:
		bool empty() const {
			return !m_packet;
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
	using PacketHandlerFunc = std::function<PacketEnums(byte_buffer&)>;

	// Packet manager creates and instiantiates new packets based on the provided id class.
	template<class PacketEnums>
	class PacketManager {
	public:
		static void init() {
			// Allow net::Packet to be serialized/deserialized from the underlying type (if its defined).
			net::serializer::bind<net::Packet>(net::Packet::serialize, net::Packet::deserialize);
		}
		template<class PacketType>
		static void bind(const PacketEnums& id) {
			m_factories.insert({ id, std::make_shared<PacketFactory<PacketType>>() });
		}
		static net::Packet create(net::byte_buffer& in) {
			PacketEnums userPacketId = m_packetHandler(in);
			net::Packet packet = (m_factories[userPacketId])->create();
			net::serializer::deserialize(packet, in);
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