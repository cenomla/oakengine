#pragma once

#include <atomic>

#include "util/byte_buffer.h"
#include "engine.h"
#include "resource_manager.h"
#include "pup.h"

namespace oak {

	template<class T>
	class Resource;

	template<class T>
	void pup(Puper &puper, Resource<T> &resource, const ObjInfo &info) {
		puper.pup(resource.id_, info);
		if (puper.getIo() == PuperIo::IN) {
			resource.res_ = nullptr;
		}
	}

	template<class T>
	class Resource {
	public:
		template<class TF> 
		friend void pup(Puper &puper, Resource<TF> &resource, const ObjInfo &info);

		Resource(size_t id) : id_{ id }, res_{ nullptr } {}

		Resource(const Resource &other) : id_{ other.id_ }, res_{ nullptr } {}
		
		void operator=(const Resource &other) {
			id_ = other.id_;
			res_ = nullptr;
		}

		const T& get() const {
			if (res_ != nullptr) {
				return *res_;
			} else {
				res_ = &Engine::inst().getSystem<ResourceManager>().require<T>(id_);
				return *res_;
			}
		}

		inline void clear() { res_ = nullptr; }
		inline size_t id() const { return id_; }

	private:
		size_t id_;
		mutable std::atomic<const T*> res_;
	};

}