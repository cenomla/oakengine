#include "scene_utils.h"

#include <string>

#include "util/stream_puper.h"
#include "util/byte_buffer.h"
#include "file_manager.h"
#include "pup.h"

namespace oak {

	void save(const oak::string& path, Scene& scene) {
		auto file = FileManager::inst().openFile(path, true);
		
		auto info = ObjInfo::make<Scene>(nullptr, "scene");

		//header:
		//version
		//entity count
		//max component count
		//text:
		//for each entity:
		//	active
		//	component count
		//	for each component:
		//		component id
		//		component data

		StreamPuper puper{ &file };

		size_t version = 0;
		size_t entityCount = scene.getEntityCount();
		size_t maxComponents = config::MAX_COMPONENTS;

		pup(puper, version, ObjInfo::make<size_t>(&info, "version"));
		pup(puper, entityCount, ObjInfo::make<size_t>(&info, "entityCount"));
		pup(puper, maxComponents, ObjInfo::make<size_t>(&info, "maxComponents"));

		auto& entities = scene.getEntities();

		size_t i = 0;
		for (auto& entity : entities) {
			auto& filter = scene.getComponentFilter(entity);
			auto active = scene.isEntityActive(entity);
			auto componentCount = filter.count();
			auto entityInfo = ObjInfo::make<EntityId>(&info, oak::string{ "entity[" } + std::to_string(i).c_str() + "]");
			pup(puper, active, ObjInfo::make<size_t>(&entityInfo, "active"));
			pup(puper, componentCount, ObjInfo::make<size_t>(&entityInfo, "componentCount"));
			for (size_t j = 0; j < config::MAX_COMPONENTS; j++) {
				if (filter[j]) {
					pup(puper, j, ObjInfo::make<size_t>(&entityInfo, "componentId"));
					ComponentTypeManager::inst().getHandle(j)->serialize(puper, scene.getComponent(entity, j), &entityInfo);
				}
			}
			i++;
		}

		FileManager::inst().closeFile(file);
	}

	void load(const oak::string& path, Scene& scene) {
		scene.reset();

		auto file = FileManager::inst().openFile(path);
		
		auto info = ObjInfo::make<Scene>(nullptr, "scene");

		StreamPuper puper{ &file };
		puper.setIo(PuperIo::IN);

		size_t version, entityCount, maxComponents;

		pup(puper, version, ObjInfo::make<size_t>(&info, "version"));
		pup(puper, entityCount, ObjInfo::make<size_t>(&info, "entityCount"));
		pup(puper, maxComponents, ObjInfo::make<size_t>(&info, "maxComponents"));

		oak_assert(maxComponents == config::MAX_COMPONENTS);

		for (size_t i = 0; i < entityCount; i++) {
			auto entityInfo = ObjInfo::make<EntityId>(&info, oak::string{ "entity[" } + std::to_string(i).c_str() + "]");
			EntityId entity = scene.createEntity();
			bool active;
			size_t componentCount;
			pup(puper, active, ObjInfo::make<bool>(&entityInfo, "active"));
			pup(puper, componentCount, ObjInfo::make<size_t>(&entityInfo, "componentCount"));
			for (size_t j = 0; j < componentCount; j++) {
				size_t componentId;
				pup(puper, componentId, ObjInfo::make<size_t>(&entityInfo, "componentId"));
				auto comp = scene.addComponent(entity, componentId);
				ComponentTypeManager::inst().getHandle(componentId)->serialize(puper, comp, &entityInfo);
			}
			if (active) {
				scene.activateEntity(entity);
			}
		}
		FileManager::inst().closeFile(file);
	}

}