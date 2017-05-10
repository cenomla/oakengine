#pragma once

#include <cstddef>

namespace oak {

	namespace util {
		
		template<typename TBase>
		struct type_id_base {

			static size_t nextTypeId;
		};

		template<typename TBase, typename T>
		struct type_id : public type_id_base<TBase> {
			const static size_t id;
		};

		template<typename TBase>
		size_t type_id_base<TBase>::nextTypeId = 0;

		template<typename TBase, typename T>
		const size_t type_id<TBase, T>::id = type_id_base<TBase>::nextTypeId++;

	}

}