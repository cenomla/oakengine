#include "console.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/gl_api.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_manager.h>
#include <input_events.h>
#include <scene_utils.h>
#include <util/string_util.h>

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
	oak::graphics::shader::bind(shader);
	oak::graphics::shader::setUniform(shader, "text_width", 0.4f);
	oak::graphics::shader::setUniform(shader, "text_edge", 0.4f);
	oak::graphics::shader::setUniform(shader, "border_width", 0.1f);
	oak::graphics::shader::setUniform(shader, "border_edge", 0.1f);
	oak::graphics::TextureInfo texInfo;
	auto& tex = texHandle.add("dejavu", oak::graphics::texture::create("/res/fonts/dejavu_sans/atlas.png", texInfo));
	auto& material = matHandle.add("dejavu", &shader, &tex);
	auto& font = fontHandle.add("dejavu", oak::graphics::loadFont("/res/fonts/dejavu_sans/glyphs.fnt"));

	console_ = scene_->createEntity();
	oak::addComponent<TransformComponent>(console_, *scene_, glm::translate(glm::scale(glm::mat3{ 1.0f }, glm::vec2{ 0.15f }), glm::vec2{ 128.0f, 16.0f }));
	oak::addComponent<TextComponent>(console_, *scene_, "console: ", oak::Resource<oak::graphics::Font>{ "dejavu" }, oak::Resource<oak::graphics::Material>{ "dejavu" }, 0u);
}

void Console::terminate() {

}

static void save(const oak::vector<oak::string>& tokens, oak::Scene *scene) {
	log_print_out("saving game...");
	oak::save(tokens[2], *scene);
}

static void load(const oak::vector<oak::string>& tokens, oak::Scene *scene) {
	log_print_out("loading game...");
	oak::load(tokens[2], *scene);
}

static void edit(const oak::vector<oak::string>& tokens, oak::Scene *scene) {
	log_print_out("entering edit mode...");
}

struct Command {
	const char *name = nullptr;
	void(*exec)(const oak::vector<oak::string>&, oak::Scene*) = nullptr;
};

static const Command commands[] = {
	{ "save", save },
	{ "load", load },
	{ "edit", edit }
};

void Console::run() {
	auto& tc = oak::getComponent<TextComponent>(console_, *scene_);

	bool isActive = scene_->isEntityActive(console_);

	//get character events 
	if (isActive) {
		for (auto& evt : oak::EventManager::inst().getQueue<oak::TextEvent>()) {
			auto c = static_cast<char>(evt.codepoint);
			tc.text.push_back(c);
			oak::InputManager::inst().setKey(toupper(c), oak::action::released);
		}
	}

	//get key events
	for (auto& evt : oak::EventManager::inst().getQueue<oak::KeyEvent>()) {
		if (evt.action != oak::action::released) {
			if (isActive) {
				//BACKSPACE backspaces
				if (evt.key == oak::key::backspace) {
					if (tc.text.size() > 9) {
						if (evt.mods & oak::mod_bit::control) {
							size_t p = tc.text.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
							p = p > 9 ? p : 9;
							if (p == oak::string::npos) {
								p = 9;
							}
							tc.text.erase(std::begin(tc.text) + p, std::end(tc.text));
						} else {
							tc.text.pop_back();
						}
					}
				}
			}
		}
		if (evt.action == oak::action::pressed) {
			//ESC toggles console
			if (evt.key == oak::key::esc) {
				if (scene_->isEntityActive(console_)) {
					scene_->deactivateEntity(console_);
				} else {
					scene_->activateEntity(console_);
				}
			}
			if (isActive) {
				
				//ENTER runs command
				if (evt.key == oak::key::enter) {
					oak::vector<oak::string> tokens;
					oak::util::splitstr(tc.text, " ", tokens);
					
					for (auto& command : commands) {
						if (strcmp(tokens[1].c_str(), command.name) == 0) {
							command.exec(tokens, scene_);
							break;
						}
					}
					
					tc.text = "console: ";
				}
			}
		}
	}

}