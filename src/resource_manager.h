#pragma once

#include "util/typeid.h"

namespace oak {

	class ResourceManager {
	public:

		template<typename T>
		addResource(const std::string &name, T&& resource) {
			size_t id
		}

		template<typename T>
		const T& getResource(const std::string &name, T&& resource);
		
	private:
		std::array<std::unordered_map<std::string, ResourceHandleBase*>, RES_COUNT> resources_;
	}

}