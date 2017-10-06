#pragma once

#include <cstring>
#include <type_traits>

#include "allocator.h"

namespace oak {

	template<class T>
	struct Array {

		typedef T value_type; 

		void init(size_t size) {
			resize(size);
		}

		void destroy() {
			clear();
			if (data) {
				size_t size = ptrutil::align_size(sizeof(T), allocator->getAlignment());
				allocator->deallocate(data, capacity * size);
				data = nullptr;
			}
			capacity = 0;
		}

		void resize(size_t nsize) {
			if (!allocator || nsize <= capacity) { return; }
			size_t size = ptrutil::align_size(sizeof(T), allocator->getAlignment());
			auto mem = static_cast<T*>(allocator->allocate(nsize * size));
			if (data) {
				std::memcpy(mem, data, capacity);
				allocator->deallocate(data, capacity * size);
			}
			data = mem;
			capacity = nsize;
		}

		void push(const T& v) {
			if (size == capacity) {
				resize(capacity == 0 ? 1 : capacity * 2);
			}
			data[size++] = v;
		}

		void remove(int index) {
			//swap and pop
			T& v = data[size - 1];
			data[index] = v;
			if constexpr(!std::is_pod<T>::value) {
				v.~T();
			}
			size--;
		}

		void clear() {
			if constexpr(!std::is_pod<T>::value) {
				for (size_t i = 0; i < size; i++) {
					data[i].~T();
				}
			}
			size = 0;
		}

		T& operator[](size_t index) { return data[index]; }
		const T& operator[](size_t index) const { return data[index]; }

		Allocator *allocator = nullptr;
		T *data = nullptr;
		size_t size = 0;
		size_t capacity = 0;
	};

}

