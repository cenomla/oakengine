#pragma once

#include <vector>
#include <deque>
#include <string>
#include <unordered_map>
#include <scoped_allocator>

#include "allocator.h"

namespace oak {

	template<class T>
	using oalloc = std::scoped_allocator_adaptor<OakAllocator<T>>;

	template<class T>
	using vector = std::vector<T, oalloc<T>>;

	template<class T>
	using deque = std::deque<T, oalloc<T>>;

	using string = std::string;
	
	template<class U, class T>
	using unordered_map = std::unordered_map<U, T, std::hash<U>, std::equal_to<U>, oalloc<std::pair<const U, T>>>;
}