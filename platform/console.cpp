#include "console.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/gl_api.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_events.h>
#include <scene_utils.h>

#include "components.h"

Console::Console(oak::Scene *scene) : scene_{ scene } {}


void Console::init() {
	auto& shHandle = oak::ResourceManager::inst().get<oak::graphics::Shader>();
	auto& texHandle = oak::ResourceManager::inst().get<oak::graphics::Texture>();
	auto& matHandle = oak::ResourceManager::inst().get<oak::graphics::Material>();
	auto& fontHandle = oak::ResourceManager::inst().get<oak::graphics::Font>();

	//load font rendering stuffs
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "/res/shaders/font/vert.glsl";
	shaderInfo.fragment = "/res/shaders/font/frag.glsl";
	auto& shader = shHandle.add("font", oak::graphics::shader::create(shaderInfo));
	oak::graphics::TextureInfo texInfo;
	auto& tex = texHandle.add("dejavu", oak::graphics::texture::create("/res/fonts/dejavu_sans/atlas.png", texInfo));
	auto& material = matHandle.add("dejavu", &shader, &tex);
	auto& font = fontHandle.add("dejavu", oak::graphics::loadFont("/res/fonts/dejavu_sans/glyphs.fnt"));

	console_ = scene_->createEntity();
	oak::addComponent<TransformComponent>(console_, *scene_, glm::scale(glm::mat3{ 1.0f }, glm::vec2{ 0.3f }));
	oak::addComponent<TextComponent>(console_, *scene_, "console: ", &font, &material, 0u);
}

void Console::terminate() {

}

void Console::run() {
	auto& tc = oak::getComponent<TextComponent>(console_, *scene_);

	//get character events 
	for (auto& evt : oak::EventManager::inst().getQueue<oak::TextEvent>()) {
		tc.text.push_back(static_cast<char>(evt.codepoint));
	}

	//get key events
	for (auto& evt : oak::EventManager::inst().getQueue<oak::KeyEvent>()) {
		if (evt.action == oak::action::pressed) {
			//ESC toggles console
			if (evt.key == oak::key::esc) {
				if (scene_->isEntityActive(console_)) {
					scene_->deactivateEntity(console_);
				} else {
					scene_->activateEntity(console_);
				}
			}
			//ENTER runs command
			if (evt.key == oak::key::enter) {
				
				tc.text = "console: ";
			}
		}
	}

}