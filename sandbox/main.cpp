#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <graphics/opengl/gl_texture_atlas.h>
#include <graphics/opengl/gl_api.h>
#include <graphics/opengl/gl_buffer_storage.h>
#include <graphics/opengl/gl_shader.h>
#include <graphics/opengl/gl_framebuffer.h>

#include <graphics/render_system.h>
#include <graphics/model.h>
#include <log.h>
#include <system_manager.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_manager.h>
#include <view_system.h>
#include <resource.h>
#include <component_storage.h>
#include <core_components.h>
#include <components.h>
#include <input_events.h>
#include <scene_events.h>
#include <update_events.h>
#include <input.h>
#include <prefab.h>
#include <scene_utils.h>

#include "component_ext.h"
#include "deferred_renderer.h"
#include "gui_renderer.h"

#include <glad/glad.h>

class MovementSystem : public oak::System {
public:
	MovementSystem(oak::Scene& scene) : scene_{ &scene } {}

	void init() override {
		cache_.requireComponent<oak::TransformComponent>();
		cache_.requireComponent<oak::PrefabComponent>();
		cache_.requirePrefab(std::hash<oak::string>{}("player"));
	}

	void run() override {
		cache_.update(*scene_);

		auto& ts = oak::getComponentStorage<oak::PositionComponent>(*scene_);

		for (const auto& entity : cache_.entities()) {
			auto& tc = oak::getComponent<oak::PositionComponent>(ts, entity);
			
			if (oak::InputManager::inst().getAction("move_up")) {
				tc.position.y -= 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_down")) {
				tc.position.y += 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_left")) {
				tc.position.x -= 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_right")) {
				tc.position.x += 4.0f;
			}
		}
	}

private:
	oak::EntityCache cache_;
	oak::Scene *scene_;
};

struct stdoutstream : public oak::log::Stream {
	void write(const void *source, size_t size) override {
		fwrite(source, 1, size, stdout);
	}
};

struct logfilestream : public oak::log::Stream {
	void open() {
		if (file == nullptr) {
			file = fopen("log", "w");
		}
	}

	void close() {
		if (file != nullptr) {
			fclose(file);
			file = nullptr;
		}
	}

	void write(const void *source, size_t size) override {
		if (file != nullptr) {
			fwrite(source, 1, size, file);
			fflush(file);
		}
	}

	FILE *file = nullptr;
};

bool isRunning = false;

int main(int argc, char** argv) {
	//setup log
	stdoutstream sos;
	logfilestream lfs;
	lfs.open();
	oak::log::cout.addStream(&sos);
	oak::log::cout.addStream(&lfs);
	oak::log::cwarn.addStream(&sos);
	oak::log::cwarn.addStream(&lfs);
	oak::log::cerr.addStream(&sos);
	oak::log::cerr.addStream(&lfs);

	//init engine managers
	oak::EventManager evtManager;
	oak::InputManager inputManager;
	oak::SystemManager sysManager;
	oak::ComponentTypeManager chs;
	oak::ResourceManager resManager;

	inputManager.bind("move_forward", oak::key::w, true);
	inputManager.bind("move_backward", oak::key::s, true);
	inputManager.bind("strafe_left", oak::key::a, true);
	inputManager.bind("strafe_right", oak::key::d, true);
	inputManager.bind("move_up", oak::key::space, true);
	inputManager.bind("move_down", oak::key::lshift, true);

	//add all events
	evtManager.addQueue<oak::EntityCreateEvent>();
	evtManager.addQueue<oak::EntityDestroyEvent>();
	evtManager.addQueue<oak::EntityActivateEvent>();
	evtManager.addQueue<oak::EntityDeactivateEvent>();
	evtManager.addQueue<oak::WindowCreateEvent>();
	evtManager.addQueue<oak::WindowCloseEvent>();
	evtManager.addQueue<oak::WindowResizeEvent>();
	evtManager.addQueue<oak::KeyEvent>();
	evtManager.addQueue<oak::ButtonEvent>();
	evtManager.addQueue<oak::CursorEvent>();
	evtManager.addQueue<oak::CursorModeEvent>();
	evtManager.addQueue<oak::TextEvent>();
	evtManager.addQueue<oak::TickEvent>();
	evtManager.addQueue<oak::SimulateEvent>();

	//create the scene
	oak::Scene scene;

	//create all the systems
	oak::graphics::GLApi gl_api;

	//create the rendering system
	oak::graphics::RenderSystem renderSystem{ scene, gl_api };

	//basic test renderer
	DeferredRenderer sceneRenderer;
	GuiRenderer guiRenderer;
	renderSystem.pushLayerBack(sceneRenderer);
	renderSystem.pushLayerBack(guiRenderer);

	//define vertex attribute layouts that will be used in the scene
	oak::graphics::AttributeLayout _3dlayout{ oak::vector<oak::graphics::AttributeType>{ 
		oak::graphics::AttributeType::POSITION,
		oak::graphics::AttributeType::NORMAL,
		oak::graphics::AttributeType::UV
	} };

	oak::graphics::AttributeLayout _2dlayout{ oak::vector<oak::graphics::AttributeType>{ 
		oak::graphics::AttributeType::POSITION2D,
		oak::graphics::AttributeType::UV
	} };

	oak::graphics::GLBufferStorage _3dstorage;
	oak::graphics::GLBufferStorage _2dstorage;

	renderSystem.batcher_.addBufferStorage(&_3dlayout, &_3dstorage);
	renderSystem.batcher_.addBufferStorage(&_2dlayout, &_2dstorage);
	

	MovementSystem movementSystem{ scene };
	//add them to the system manager
	sysManager.addSystem(renderSystem, "render_system");
	sysManager.addSystem(movementSystem, "movement_system");

	//create component type handles
	chs.addHandle<oak::EventComponent>("event");
	chs.addHandle<oak::PrefabComponent>("prefab");
	chs.addHandle<oak::TransformComponent>("transform");
	chs.addHandle<oak::SpriteComponent>("sprite");
	chs.addHandle<oak::TextComponent>("text");
	chs.addHandle<oak::AABB2dComponent>("aabb2d");
	chs.addHandle<OccluderComponent>("occluder");
	chs.addHandle<LightComponent>("light");

	//create component storage
	oak::ComponentStorage eventStorage{ "event" };
	oak::ComponentStorage prefabStorage{ "prefab" };
	oak::ComponentStorage transformStorage{ "transform" };
	oak::ComponentStorage spriteStorage{ "sprite" };
	oak::ComponentStorage textStorage{ "text" };
	oak::ComponentStorage aabbStorage{ "aabb2d" };
	oak::ComponentStorage occluderStorage{ "occluder" };
	oak::ComponentStorage lightStorage{ "light" };

	//add component storage to scene
	scene.addComponentStorage(eventStorage);
	scene.addComponentStorage(prefabStorage);
	scene.addComponentStorage(transformStorage);
	scene.addComponentStorage(spriteStorage);
	scene.addComponentStorage(textStorage);
	scene.addComponentStorage(aabbStorage);
	scene.addComponentStorage(occluderStorage);
	scene.addComponentStorage(lightStorage);

	//init the test renderer
	sceneRenderer.init();
	guiRenderer.init();

	//initialize the buffer storage
	_3dstorage.create(&_3dlayout);
	_2dstorage.create(&_2dlayout);

	//setup uniforms
	struct {
		glm::mat4 proj;
		glm::mat4 view;
	} block;
	block.proj = glm::perspective(70.0f, 1280.0f / 720.0f, 0.5f, 500.0f);
	block.view = glm::lookAt(glm::vec3{ 16.0f, 8.0f, 16.0f }, glm::vec3{ 4.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

	struct {
		glm::mat4 invProj;
		glm::mat4 proj;
	} iblock;
	iblock.invProj = glm::inverse(block.proj);
	iblock.proj = block.proj;

	struct {
		glm::mat4 proj;
		glm::mat4 view;
	} oblock;
	oblock.view = glm::mat4{ 1.0f };
	oblock.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	oak::graphics::GLBuffer ubo{ GL_UNIFORM_BUFFER };
	ubo.create();
	ubo.bindBufferBase(0);

	oak::graphics::GLBuffer iubo{ GL_UNIFORM_BUFFER };
	iubo.create();
	iubo.bind();
	iubo.data(sizeof(iblock), &iblock, GL_STREAM_DRAW);
	iubo.bindBufferBase(2);

	oak::graphics::GLBuffer oubo{ GL_UNIFORM_BUFFER };
	oubo.create();
	oubo.bind();
	oubo.data(sizeof(oblock), &oblock, GL_STATIC_DRAW);
	oubo.bindBufferBase(1);

	//lights

	struct {
		glm::vec3 pos;
		float radius;
		glm::vec3 color;
		float padding = 0.0f;
	} lights[8];

	glm::vec3 lpos[8] = {
		{ 16.0f, 5.0f, 34.0f },
		{ 16.0f, 11.0f, 34.0f },
		{ 16.0f, 5.0f, 40.0f },
		{ 16.0f, 11.0f, 40.0f },
		{ 32.0f, 8.0f, 52.0f },
		{ 35.0f, 32.0f, 37.0f },
		{ 48.0f, 8.0f, 37.0f },
		{ 1.0f, 1.0f, 1.0f }
	};
	
	for (int i = 0; i < 8; i++) {
		lights[i].radius = 128.0f;
		lights[i].color = { 1.0f, 1.0f, 1.0f };
	}

	oak::graphics::GLBuffer lubo{ GL_UNIFORM_BUFFER };
	lubo.create();
	lubo.bindBufferBase(4);
	
	//shader setup
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "core/graphics/shaders/deferred/geometry/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/geometry/frag.glsl";
	auto& sh_geometry = resManager.add<oak::graphics::Shader>("sh_geometry", oak::graphics::GLShader::create(shaderInfo));
	shaderInfo.vertex = "core/graphics/shaders/forward/pass2d/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/forward/pass2d/frag.glsl";
	auto& sh_pass2d = resManager.add<oak::graphics::Shader>("sh_pass2d", oak::graphics::GLShader::create(shaderInfo));
	//textures
	auto& tex_pbrBase = resManager.add<oak::graphics::Texture>("tex_pbrBase", oak::graphics::GLTexture::create("sandbox/res/textures/pbr_rust/color.png"));
	auto& tex_pbrNormal = resManager.add<oak::graphics::Texture>("tex_pbrNormal", oak::graphics::GLTexture::create("sandbox/res/textures/pbr_rust/normal.png"));
	auto& tex_pbrMetallic = resManager.add<oak::graphics::Texture>("tex_pbrMetallic", oak::graphics::GLTexture::create("sandbox/res/textures/pbr_rust/metalness.png", 1));
	auto& tex_pbrRoughness = resManager.add<oak::graphics::Texture>("tex_pbrRoughness", oak::graphics::GLTexture::create("sandbox/res/textures/pbr_rust/roughness.png", 1));

	auto& tex_character = resManager.add<oak::graphics::Texture>("tex_character", oak::graphics::GLTexture::create("sandbox/res/textures/character.png"));

	//materials
	auto& mat_box = resManager.add<oak::graphics::Material>("mat_box", &_3dlayout, &sh_geometry, &tex_pbrBase, &tex_pbrRoughness, &tex_pbrMetallic);
	auto& mat_overlay = resManager.add<oak::graphics::Material>("mat_overlay", &_2dlayout, &sh_pass2d, &tex_character);

	//meshes
	auto& mesh_box = resManager.add<oak::graphics::Model>("model_box");
	mesh_box.load("sandbox/res/models/box.obj");

	auto& mesh_floor = resManager.add<oak::graphics::Mesh>("mesh_floor");

	oak::vector<oak::graphics::Mesh::Vertex> vertices = {
		{ glm::vec3{ 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 0.0f } },
		{ glm::vec3{ 0.0f, 0.0f, 64.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f } },
		{ glm::vec3{ 64.0f, 0.0f, 64.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f } },
		{ glm::vec3{ 64.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f } }
	};

	oak::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	mesh_floor.setData(vertices, indices);

	renderSystem.batcher_.addMesh(glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 32.0f, 2.0f, 32.0f }), &mesh_box.getMeshes()[0], &mat_box, 0);
	renderSystem.batcher_.addMesh(glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }), &mesh_floor, &mat_box, 0);

	//create entities
	oak::EntityId entity = scene.createEntity();
	scene.activateEntity(entity);

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	inputManager.update();

	//cam
	float roth = 180.0f, rotv = 0.0f;
	float px, py;
	glm::vec3 pos{ 16.0f, 8.0f, 16.0f };
	glm::vec3 look{ glm::quat{ glm::vec3{ rotv, roth, 0.0f } } * glm::vec3{ 0.0f, 0.0f, -1.0f } };
	{
		double mx, my;
		inputManager.getCursorPos(&mx, &my);
		px = static_cast<float>(mx);
		py = static_cast<float>(my);
	}
	oak::CursorMode cmode = oak::CursorMode::NORMAL;
	evtManager.getQueue<oak::CursorModeEvent>().emit({ cmode });


	//main game loop
	isRunning = true;
	while (isRunning) {
		if (inputManager.getKey(oak::key::esc)) {
			if (cmode == oak::CursorMode::NORMAL) {
				cmode = oak::CursorMode::DISABLED;
			} else {
				cmode = oak::CursorMode::NORMAL;
			}

			evtManager.getQueue<oak::CursorModeEvent>().emit({ cmode });
			inputManager.setKey(oak::key::esc, 0);
		}
		inputManager.update();
		//create / destroy / activate / deactivate entities
		scene.update();

		//temp camera
		//look
		if (cmode == oak::CursorMode::DISABLED) {
			for (const auto& evt : evtManager.getQueue<oak::CursorEvent>()) {
				roth -= glm::radians(evt.x - px) / 6.0f;
				rotv -= glm::radians(evt.y - py) / 6.0f;
				px = evt.x;
				py = evt.y;

				look = glm::quat{ glm::vec3{ rotv, roth, 0.0f } } * glm::vec3{ 0.0f, 0.0f, -1.0f };
			}
		} else {
			for (const auto& evt : evtManager.getQueue<oak::CursorEvent>()) {
				px = evt.x;
				py = evt.y;
			}
		}
		//move
		if (inputManager.getAction("move_forward")) {
			pos += glm::vec3{ look.x, 0.0f, look.z };
		}
		if (inputManager.getAction("move_backward")) {
			pos -= glm::vec3{ look.x, 0.0f, look.z };
		}
		if (inputManager.getAction("strafe_left")) {
			pos -= glm::cross(glm::vec3{ look.x, 0.0f, look.z }, glm::vec3{ 0.0f, 1.0f, 0.0f });
		}
		if (inputManager.getAction("strafe_right")) {
			pos += glm::cross(glm::vec3{ look.x, 0.0f, look.z }, glm::vec3{ 0.0f, 1.0f, 0.0f });
		}
		if (inputManager.getAction("move_up")) {
			pos.y += 0.4f;
		}
		if (inputManager.getAction("move_down")) {
			pos.y -= 0.4f;
		}

		block.view = glm::lookAt(pos, pos + look, glm::vec3{ 0.0f, 1.0f, 0.0f });

		ubo.bind();
		ubo.data(sizeof(block), &block, GL_STREAM_DRAW);

		for (int i = 0; i < 8; i++) {
			lights[i].pos = glm::vec3{ block.view * glm::vec4{ lpos[i], 1.0f} };
		}

		lubo.bind();
		lubo.data(sizeof(lights), &lights, GL_STREAM_DRAW);
	

		movementSystem.run();

		renderSystem.run();

		//check for exit
		if (!evtManager.getQueue<oak::WindowCloseEvent>().empty()) {
			isRunning = false; 
		}

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;

		printf("dt: %f\n", dt.count());

		//do engine things
		evtManager.clear();
		oak::oalloc_frame.clear();
	}

	//clean up
	scene.reset();

	lfs.close();

	return 0;
}
