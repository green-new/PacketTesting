#pragma once
#include "def.hpp"
#include "buffer.hpp"
#include <unordered_map>
#include <functional>

namespace net {

	template<class Serializable>
	struct DszPair {
		DszPair(Serializable& object, byte_buffer& output)
			: m_object(object), m_output(output) { }
		DszPair(const DszPair<Serializable>& copy) = delete;
		DszPair& operator=(const DszPair<Serializable>& copy) = delete;
		DszPair(DszPair<Serializable>&& move) = delete;
		DszPair& operator=(DszPair<Serializable>&& move) = delete;
		~DszPair() = default;

		Serializable& m_object;
		byte_buffer& m_output;
	};

	template<class Serializable>
	struct SzPair {
		SzPair(const Serializable& object, byte_buffer& output)
			: m_object(object), m_output(output) { }
		SzPair(const SzPair<Serializable>& copy) = delete;
		SzPair& operator=(const SzPair<Serializable>& copy) = delete;
		SzPair(SzPair<Serializable>&& move) = delete;
		SzPair& operator=(SzPair<Serializable>&& move) = delete;
		~SzPair() = default;

		const Serializable& m_object;
		byte_buffer& m_output;
	};
	// Remove Serializable, one func type alias, should be template instead, not runtime std function. policy pattern for sz and dsz.
	template<class Serializable>
	using SzFunc = std::function<void(const Serializable&, byte_buffer&)>;

	template<class Serializable>
	using DszFunc = std::function<void(Serializable&, byte_buffer&)>;

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
		void serialize(SzPair<Serializable>&& pair) { 
			m_serializationFunc(pair.m_object, pair.m_output); 
		}
		void deserialize(DszPair<Serializable>&& pair) { 
			m_deserializationFunc(pair.m_object, pair.m_output);
		}
	private:
		SzFunc<Serializable> m_serializationFunc;
		DszFunc<Serializable> m_deserializationFunc;
	};
	
	namespace serializer {
		inline size_t s_next_type_id{};
		inline std::unordered_map<size_t, std::shared_ptr<SzFuncGroupRoot>> s_functors{};
		
		template<typename T>
		inline size_t get_type_id() {
			static size_t type_id = s_next_type_id++;
			return type_id;
		}
		
		template<class Serializable>
		inline void bind(SzFunc<Serializable> szFunc, DszFunc<Serializable> dszFunc) {
			s_functors.insert({ get_type_id<Serializable>(), std::make_shared<SzFuncGroup<Serializable>>(szFunc, dszFunc) });
		}
		
		template<class Serializable>
		inline byte_buffer serialize(const Serializable& object) {
			byte_buffer b{};
			serialize(std::forward(object), b);
			return b;
		}
		
		template<class Serializable>
		inline void serialize(const Serializable& object, byte_buffer& output) {
			std::static_pointer_cast<SzFuncGroup<Serializable>>(s_functors[get_type_id<Serializable>()])->serialize(SzPair(object, output));
		}
		
		template<class Serializable>
		inline Serializable deserialize(byte_buffer& input) {
			Serializable s{};
			deserialize(s, input);
			return s;
		}
		
		template<class Serializable>
		inline void deserialize(Serializable& object, byte_buffer& input) {
			std::static_pointer_cast<SzFuncGroup<Serializable>>(s_functors[get_type_id<Serializable>()])->deserialize(DszPair(object, input));
		}
		
	}
}