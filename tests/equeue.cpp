#include <event_queue.h>
#include <log.h>

struct TEvent {
	size_t size = 8;
	size_t index = 59;
	uint8_t byte = 109;
};

int main(int argc, char **argv) {

	oak::EventQueue<TEvent> queue;

	for (size_t i = 0; i < 1024; i++) {
		queue({ i * i * i, i, i % 255 });
	}

	size_t i = 0;
	{
		auto &&range = queue;
		auto begin = range.begin();
		auto end = range.end();
		for (; begin != end; ++begin) {
			const auto& event = *begin;
			if (event.size != (i * i * i) || event.index != i || event.byte != i % 255) {
				return -1;
			}
			i++;
		}
	}

	queue.clear();


	return 0;

}