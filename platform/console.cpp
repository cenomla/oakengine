#include "console.h"

#include <resource_manager.h>
#include <event_manager.h>
#include <input_events.h>
#include <scene_utils.h>

#include "components.h"

Console::Console(oak::Scene *scene) : scene_{ scene } {}


void Console::init() {
	auto& matHandle = oak::ResourceManager::inst().get<oak::graphics::Material>();


	console_ = scene_->createEntity();
	oak::addComponent<TransformComponent>(console_, *scene_);
	oak::addComponent<TextComponent>(console_, *scene_);
}

void Console::terminate() {

}

void Console::run() {

	//get character events 
	for (auto& evt : oak::EventManager::inst().getQueue<oak::TextEvent>()) {

	}

	//get key events
	for (auto& evt : oak::EventManager::inst().getQueue<oak::KeyEvent>()) {
		//T toggles console

		//
	}

}