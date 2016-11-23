#pragma once

#include <atomic>

#include "util/byte_buffer.h"
#include "util/pupper.h"
#include "engine.h"
#include "resource_manager.h"

namespace oak {

	template<class T>
	class Resource {
	public:
		template<class TF>
		friend void pup(util::Pupper &pupper, Resource<TF> &resource, const util::ObjInfo &info);

		Resource(size_t id) : id_{ id }, res_{ nullptr } {}
		
		Resource(const Resource &other) : id_{ other.id_ }, res_{ nullptr } {}
		void operator=(const Resource &other) { id_ = other.id_; res_ = nullptr; }

		const T* get() const {
			if (res_ != nullptr) {
				return res_;
			} else {
				res_ = &Engine::inst().getSystem<ResourceManager>().require<T>(id_);
				return res_;
			}
		}

		inline void clear() { res_ = nullptr; }
		inline size_t id() const { return id_; }

	private:
		size_t id_;
		mutable std::atomic<const T*> res_;
	};

	namespace util {
		template<class T>
		void pup(Pupper &pupper, Resource<T> &resource, const ObjInfo &info) {
			pupper.pup(resource.id_, info);
		}
	}

}