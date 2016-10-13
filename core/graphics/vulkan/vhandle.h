#pragma once

#include <functional>

namespace oak::graphics {

	template <class T>
	class VHandle {
	public:
		VHandle() : obj_{ VK_NULL_HANDLE } {};
		VHandle(std::function<void(T)> &&func) : obj_{ VK_NULL_HANDLE }, deleter_{ func } {};

		~VHandle() { cleanup(); }

		const T* operator &() const {
			return &obj_;
		}

		operator T() const {
			return obj_;
		}

		T* replace() {
			cleanup();
			return &obj_;
		}

		void operator=(T rhs) {
			cleanup();
			obj_ = rhs;
		}

		template<typename N>
		bool operator==(N rhs) {
			return obj_ == T(rhs);
		}

	private:
		T obj_;
		std::function<void(T)> deleter_;


		void cleanup() {
			if (obj_ != VK_NULL_HANDLE) {
				deleter_(obj_);
			}
			obj_ = VK_NULL_HANDLE;
		}
	};

}