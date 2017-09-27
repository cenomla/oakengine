#include "event_queue.h"

namespace oak {

	EventQueueBase::EventQueueBase(const TypeInfo *tinfo) :
		alignedSize{ ptrutil::alignSize(tinfo->size, 16) },
		size{ 0 },
		allocator{ &oalloc_freelist, 256 * alignedSize, 16 } {}

	void EventQueueBase::clear() {
		allocator.clear();
		size = 0;
	}

	bool EventQueueBase::empty() {
		return size == 0;
	}

	void* EventQueueBase::next() {
		size++;
		return allocator.allocate(alignedSize);
	}
}
