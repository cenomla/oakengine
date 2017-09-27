#include "event_queue.h"

namespace oak {

	EventQueue::EventQueue(const TypeInfo *tinfo) :
		alignedSize{ ptrutil::alignSize(tinfo->size, 16) },
		size{ 0 },
		allocator{ &oalloc_freelist, 256 * alignedSize, 16 } {}

	void EventQueue::clear() {
		allocator.clear();
		size = 0;
	}

	void EventQueue::empty() {
		return size == 0;
	}

	void* EventQueue::emit() {
		size++;
		return allocator.allocate(alignedSize);
	}
}
