#include "save_funcs.h"

#include "util/byte_buffer_puper.h"
#include "config.h"
#include "entity.h"
#include "engine.h"

namespace oak::save {

	void streamEntity(SaveManager& sm, Entity& data) {
		EntityManager& manager = Engine::inst().getSystem<EntityManager>();
		ComponentHandleStorage& storage = Engine::inst().getSystem<ComponentHandleStorage>();
		auto byteBuffer = sm.getBuffer();

		ByteBufferPuper puper{ byteBuffer };

		if ((sm.flags() & SaveManager::LOAD) == SaveManager::LOAD) {
			puper.setIo(PuperIo::IN);
		}

		//stream entity id
		uint64_t id = data.id();
		pup(puper, id, ObjInfo{ "id" });
		//calculate number of components and stream it
		size_t ncomp = 0;
		for (size_t i = 0; i < config::MAX_COMPONENTS; i++) {
			if (manager.hasComponent(data.index(), i)) {
				ncomp ++;
			}
		}
		pup(puper, ncomp, ObjInfo{ "ncomp" });
		//get components
		oak::vector<size_t> comps{ oak::frameAllocator };
		if ((sm.flags() & SaveManager::SAVE) == SaveManager::SAVE) {
			comps.reserve(ncomp);
			for (size_t i = 0; i < config::MAX_COMPONENTS; i++) {
				if (manager.hasComponent(data.index(), i)) {
					comps.push_back(i);
				}
			}
		}

		//stream every component
		for (size_t i = 0; i < ncomp; i++) {
			size_t cid = comps.empty() ? 0xFFFFFFFF : comps[i];
			pup(puper, cid, ObjInfo{ "cid" });

			void* comp = nullptr;
			if (manager.hasComponent(data.index(), cid)) {
				comp = manager.getComponent(data.index(), cid);
			} else {
				comp = manager.addComponent(data.index(), cid);
			}

			auto chandle = storage.getHandle(cid);
			chandle->pupObject(puper, comp, ObjInfo{ chandle->getName() });
		}
	}

}