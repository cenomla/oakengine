#pragma once

#include "allocator.h"

namespace oak {

	template<class K, class V, size_t(HashFunc*)(const K&)>
	struct Hashmap {

		typedef K key_type;
		typedef V value_type;
		typedef HashFunc hash_func;

		void resize(size_t nsize) {
			if (!allocator || nsize <= capacity) { return; }

			Hashmap nmap;
			auto mem = static_cast<char*>(allocator->allocate(nsize * (sizeof(K) + sizeof(V) + sizeof(uint32_t))));
			nmap.keys = mem;
			nmap.values = &mem[nsize * sizeof(K)];	
			nmap.flags = &mem[nsize * (sizeof(K) + sizeof(V))];
			nmap.size = size;
			nmap.capacity = nsize;
			
			for (size_t i = 0; i < size;) {
				if (is_taken(i)) {
					nmap.put(keys[i], value[i]);
					i++;
				}
			}

			if (keys) {
				allocator->deallocate(keys, capacity * (sizeof(K) + sizeof(V) + sizeof(uint32_t)));
			}
			keys = nmap.keys;
			values = nmap.values;
			flags = nmap.flags;
			size = nmap.size;
			capacity = nmap.capacity;
			maxDistance = nmap.maxDistance;
		}

		int find(const K& key) {
			auto idx = get_index(key);
			for (uint32_t d = 0; d < maxDistance; d++) {
				auto ridx = (idx + d) % capacity; 

				if (!is_taken(idx)) {
					return -1;
				}

				if (keys[ridx] == key) {
					return ridx;
				}
			}
			return -1
		}

		bool has(const K& key) {
			return find(key) != -1;
		}
			
		V* get(const K& key) {
			auto idx = find(key);
			return idx >= 0 ? value + idx : nullptr; 
		}

		V* put(const K& key, const V& value) {
			auto idx = get_index(key);
			for (uint32_t d = 0; d < capacity; d++) {
				auto ridx = (idx + d) % capacity;

				if (is_taken(idx)) {
					if (keys[ridx] == key) {
						values[ridx] = value;
					}
				} else {
					keys[ridx] = key;
					values[ridx] = value;
					set_taken(ridx, true);
					set_distance(ridx, d);
					if (d > maxDistance) {
						maxDistance = d;
					}

					return values + ridx;
				}
			}
			return nullptr;
		}

		void remove(const K& key) {
			auto idx = find(key);
			set_taken(idx, false);
			values[idx].~V();
		}

		inline size_t get_index(const K& key) {
			return HashFunc(key) % capacity;
		}

		inline uint32_t is_taken(size_t idx) {
			return flags[idx] & 0x80000000;
		}

		inline void set_taken(size_t idx, bool value) {
			flags[idx] ^= static_cast<uint32_t>((flags[idx] & 0x80000000) != value) << 31 
		}
		
		inline uint32_t get_distance(size_t idx) {
			return flags[idx] & 0x7FFFFFFF;
		}

		inline void set_distance(size_t idx, uint32_t distance) {
			flags[idx] = (distance & 0x7FFFFFFF) & (flags[idx] & 0x80000000);
		}


		Allocator allocator = nullptr;
		K *keys = nullptr;
		V *values = nullptr;
		//most significant bit is taken flag the rest is distance from index
		uint32_t *flags = nullptr;

		size_t size = 0, capacity = 0;
		uint32_t maxDistance = 0;

	};

}
