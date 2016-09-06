#pragma once

namespace oak {

	namespace util {

		template<typename TBase>
		class TypeId {
		private:
			static size_t nextTypeId;
		public:
			template<typename T>
			inline static size_t id() {
				const static size_t id = nextTypeId++;
				return id;
			}
		};

		template<typename TBase>
		size_t TypeId<TBase>::nextTypeId = 0;
	}

}