#pragma once
#include "def.hpp"
#include "buffer.hpp"
namespace net {

	template<class Serializable>
	struct DszPair {
		DszPair(Serializable& object, ByteBuffer& output)
			: m_object(object), m_output(output) { }
		DszPair(const DszPair<Serializable>& copy) = delete;
		DszPair& operator=(const DszPair<Serializable>& copy) = delete;
		DszPair(DszPair<Serializable>&& move) = delete;
		DszPair& operator=(DszPair<Serializable>&& move) = delete;
		~DszPair() = default;

		Serializable& m_object;
		ByteBuffer& m_output;
	};

	template<class Serializable>
	struct SzPair {
		SzPair(const Serializable& object, ByteBuffer& output)
			: m_object(object), m_output(output) { }
		SzPair(const SzPair<Serializable>& copy) = delete;
		SzPair& operator=(const SzPair<Serializable>& copy) = delete;
		SzPair(SzPair<Serializable>&& move) = delete;
		SzPair& operator=(SzPair<Serializable>&& move) = delete;
		~SzPair() = default;

		const Serializable& m_object;
		ByteBuffer& m_output;
	};

	template<class Serializable>
	using SzFunc = std::function<void(const Serializable&, ByteBuffer&)>;

	template<class Serializable>
	using DszFunc = std::function<void(Serializable&, ByteBuffer&)>;

	class SzFuncGroupRoot {
	public:
		virtual ~SzFuncGroupRoot() = default;
	};

	template<class Serializable>
	class SzFuncGroup : public SzFuncGroupRoot {
	public:
		SzFuncGroup(const SzFunc<Serializable>& s, const DszFunc<Serializable>& ds)
			: m_serializationFunc(s), m_deserializationFunc(ds) { }
		SzFuncGroup(SzFunc<Serializable>&& s, DszFunc<Serializable>&& ds)
			: m_serializationFunc(std::move(s)), m_deserializationFunc(std::move(ds)) { }
		~SzFuncGroup() = default;
	public:
		inline void serialize(SzPair<Serializable>&& pair) { m_serializationFunc(pair.m_object, pair.m_output); }
		inline void deserialize(DszPair<Serializable>&& pair) { m_deserializationFunc(pair.m_object, pair.m_output); }
	private:
		SzFunc<Serializable> m_serializationFunc;
		DszFunc<Serializable> m_deserializationFunc;
	};

	class Serializer {
	public:
		Serializer() = delete;
		Serializer(const Serializer& copy) = delete;
		Serializer& operator=(const Serializer& copy) = delete;
		Serializer(Serializer&& move) = delete;
		Serializer& operator=(Serializer&& move) = delete;
		~Serializer() = default;
	public:
		template<class Serializable>
		static void bind(SzFunc<Serializable> serializeFunction, DszFunc<Serializable> deserializeFunction) {
			m_functors.insert({ typeid(Serializable), std::make_shared<SzFuncGroup<Serializable>>(serializeFunction, deserializeFunction) });
		}
		template<class Serializable>
		static void serialize(const Serializable& object, ByteBuffer& output) {
			std::type_index tn = typeid(Serializable);
			assert(m_functors.find(tn) != m_functors.end() && "Serializer for type does not exist");
			std::static_pointer_cast<SzFuncGroup<Serializable>>(m_functors[tn])->serialize(SzPair(object, output));
		}
		template<class Serializable>
		static void deserialize(Serializable& object, ByteBuffer& output) {
			std::type_index tn = typeid(Serializable);
			assert(m_functors.find(tn) != m_functors.end() && "Deserializer for type does not exist");
			std::static_pointer_cast<SzFuncGroup<Serializable>>(m_functors[tn])->deserialize(DszPair(object, output));
		}
	private:
		inline static std::unordered_map<std::type_index, std::shared_ptr<SzFuncGroupRoot>> m_functors{};
	};
}