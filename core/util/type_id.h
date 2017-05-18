#pragma once

#include <cstddef>

namespace oak {

	namespace util {
		
		template<class TBase>
		struct type_id_base {
			static size_t nextTypeId;
		};

		template<class TBase, class T>
		struct type_id : public type_id_base<TBase> {
			const static size_t id;
		};

		template<class TBase>
		size_t type_id_base<TBase>::nextTypeId = 0;

		template<class TBase, class T>
		const size_t type_id<TBase, T>::id = type_id_base<TBase>::nextTypeId++;

	}

}