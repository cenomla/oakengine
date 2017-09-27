#include <event_queue.h>
#include <event.h>
#include <oakengine.h>
#include <log.h>

struct TEvent {
	static const oak::TypeInfo typeInfo;

	size_t size = 8;
	size_t index = 59;
	uint8_t byte = 109;
};

const oak::TypeInfo TEvent::typeInfo = oak::makeEventInfo<TEvent>("tevent");

void pup(oak::Puper& puper, TEvent& data, const oak::ObjInfo& info) {}

int main(int argc, char **argv) {

	oak::EventManager evtManager;
	oak::EventTypeManager etm;

	etm.addType<TEvent>();

	evtManager.init();

	auto& queue = oak::getEventQueue<TEvent>();

	for (size_t i = 0; i < 1024; i++) {
		oak::emitEvent<TEvent>(i * i * i, i, i % 255);
	}

	printf("size: %lu\n", queue.size);

	size_t i = 0;
	{
		for (auto& event : queue) {
			if (event.size != (i * i * i) || event.index != i || event.byte != i % 255) {
				printf("i: %lu, size: %lu\n", i, event.size);
				return -1;
			}
			i++;
		}

		printf("done\ni: %lu\n", i);
	}

	queue.clear();


	return 0;

}
