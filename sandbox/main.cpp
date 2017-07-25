#include <iostream>
#include <chrono>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/quaternion.hpp>

#include <graphics/opengl/gl_api.h>
#include <graphics/buffer_storage.h>

#include <graphics/texture.h>
#include <graphics/shader.h>
#include <mesh.h>
#include <log.h>
#include <file_manager.h>
#include <system_manager.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_manager.h>
#include <component_storage.h>
#include <core_components.h>
#include <input_events.h>
#include <scene_events.h>
#include <update_events.h>
#include <input.h>
#include <prefab.h>
#include <scene_utils.h>

#include "components.h"
#include "render_system.h"
#include "deferred_renderer.h"
#include "sprite_renderer.h"

class CameraSystem : public oak::System {
public:
	CameraSystem(oak::Scene *scene) : scene_{ scene } {}

	void init() override {
		cache_.requireComponent<oak::PrefabComponent>();
		cache_.requireComponent<CameraComponent>();
		cache_.requirePrefab(std::hash<oak::string>{}("player"));
	}

	void run() override {
		cache_.update(*scene_);

		auto& cs = oak::getComponentStorage<CameraComponent>(*scene_);

		auto& inputManager = oak::InputManager::inst();

		for (const auto& entity : cache_.entities()) {
			auto& cc = oak::getComponent<CameraComponent>(cs, entity);

			//temp camera
			//look
			
			if (inputManager.getCursorMode() == oak::CursorMode::DISABLED) {
				for (const auto& evt : oak::EventManager::inst().getQueue<oak::CursorEvent>()) {
					//translate to zero
					cc.rotation.y -= glm::radians(evt.x - px) / 6.0f;
					cc.rotation.x -= glm::radians(evt.y - py) / 6.0f;

					if (cc.rotation.y >= glm::pi<float>() * 2.0f) {
						cc.rotation.y = 0.0f;
					}
					if (cc.rotation.x >= glm::pi<float>() * 2.0f) {
						cc.rotation.x = 0.0f;
					}
					px = evt.x;
					py = evt.y;

				}
			} else {
				for (const auto& evt : oak::EventManager::inst().getQueue<oak::CursorEvent>()) {
					px = evt.x;
					py = evt.y;
				}
			}
			//move
			
			if (inputManager.getAction("move_forward")) {
				cc.position += glm::quat{ glm::vec3{ 0.0f, cc.rotation.y, 0.0f } } * glm::vec3{ 0.0f, 0.0f, -1.0f };
			}
			if (inputManager.getAction("move_backward")) {
				cc.position += glm::quat{ glm::vec3{ 0.0f, cc.rotation.y, 0.0f } } * glm::vec3{ 0.0f, 0.0f, 1.0f };
			}
			if (inputManager.getAction("strafe_left")) {
				cc.position += glm::quat{ glm::vec3{ 0.0f, cc.rotation.y, 0.0f } } * glm::vec3{ -1.0f, 0.0f, 0.0f };
			}
			if (inputManager.getAction("strafe_right")) {
				cc.position += glm::quat{ glm::vec3{ 0.0f, cc.rotation.y, 0.0f } } * glm::vec3{ 1.0f, 0.0f, 0.0f };
			}
			if (inputManager.getAction("move_up")) {
				cc.position.y += 0.4f;
			}
			if (inputManager.getAction("move_down")) {
				cc.position.y -= 0.4f;
			}
			
			
		}
	}

private:
	oak::EntityCache cache_;
	oak::Scene *scene_;
	float px = 0.0f, py = 0.0f;

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
	oak::FileManager fileManager;
	oak::SystemManager sysManager;
	oak::ComponentTypeManager chs;
	oak::ResourceManager resManager;

	inputManager.bind("move_forward", oak::key::w, true);
	inputManager.bind("move_backward", oak::key::s, true);
	inputManager.bind("strafe_left", oak::key::a, true);
	inputManager.bind("strafe_right", oak::key::d, true);
	inputManager.bind("move_up", oak::key::space, true);
	inputManager.bind("move_down", oak::key::lshift, true);

	fileManager.mount("{$cwd}/sandbox/res", "/res");
	fileManager.mount("{$cwd}/core/graphics/shaders", "/res/shaders");

	//add all events
	evtManager.addQueue<oak::EntityCreateEvent>();
	evtManager.addQueue<oak::EntityDestroyEvent>();
	evtManager.addQueue<oak::EntityActivateEvent>();
	evtManager.addQueue<oak::EntityDeactivateEvent>();
	evtManager.addQueue<oak::WindowCreateEvent>();
	evtManager.addQueue<oak::WindowCloseEvent>();
	evtManager.addQueue<oak::WindowResizeEvent>();
	evtManager.addQueue<oak::FrameSizeEvent>();
	evtManager.addQueue<oak::KeyEvent>();
	evtManager.addQueue<oak::ButtonEvent>();
	evtManager.addQueue<oak::CursorEvent>();
	evtManager.addQueue<oak::CursorModeEvent>();
	evtManager.addQueue<oak::TextEvent>();
	evtManager.addQueue<oak::TickEvent>();
	evtManager.addQueue<oak::SimulateEvent>();

	//get references to resource storage containers
	auto& bufferHandle = resManager.get<oak::graphics::Buffer>();
	auto& textureHandle = resManager.get<oak::graphics::Texture>();
	auto& atlasHandle = resManager.get<oak::graphics::TextureAtlas>();
	auto& shaderHandle = resManager.get<oak::graphics::Shader>();
	auto& materialHandle = resManager.get<oak::graphics::Material>();

	//create the scene
	oak::Scene scene;

	//create all the systems
	oak::graphics::GLApi api;

	RenderSystem renderSystem{ &scene, &api };
	DeferredRenderer sceneRenderer;
	SpriteRenderer spriteRenderer;
	renderSystem.pushLayerBack(&sceneRenderer);
	renderSystem.pushLayerBack(&spriteRenderer);

	CameraSystem cameraSystem{ &scene };

	//add them to the system manager
	sysManager.addSystem(&renderSystem, "render_system");
	sysManager.addSystem(&cameraSystem, "camera_system");

	//create component type handles
	chs.addHandle<oak::EventComponent>("event");
	chs.addHandle<oak::PrefabComponent>("prefab");
	chs.addHandle<TransformComponent>("transform");
	chs.addHandle<Transform2dComponent>("transform2d");
	chs.addHandle<MeshComponent>("mesh");
	chs.addHandle<SpriteComponent>("sprite");
	chs.addHandle<TextComponent>("text");
	chs.addHandle<CameraComponent>("camera");

	//create component storage
	oak::ComponentStorage eventStorage{ "event" };
	oak::ComponentStorage prefabStorage{ "prefab" };
	oak::ComponentStorage transformStorage{ "transform" };
	oak::ComponentStorage transform2dStorage{ "transform2d" };
	oak::ComponentStorage modelStorage{ "mesh" };
	oak::ComponentStorage spriteStorage{ "sprite" };
	oak::ComponentStorage textStorage{ "text" };
	oak::ComponentStorage cameraStorage{ "camera" };

	//add component storage to scene
	scene.addComponentStorage(&eventStorage);
	scene.addComponentStorage(&prefabStorage);
	scene.addComponentStorage(&transformStorage);
	scene.addComponentStorage(&transform2dStorage);
	scene.addComponentStorage(&modelStorage);
	scene.addComponentStorage(&spriteStorage);
	scene.addComponentStorage(&textStorage);
	scene.addComponentStorage(&cameraStorage);

	//init the test renderer
	sceneRenderer.init();
	spriteRenderer.init();

	//setup uniforms
	struct {
		glm::mat4 proj;
		glm::mat4 view;
	} matrix;
	matrix.proj = glm::perspective(glm::radians(90.0f), (1920.0f)/(1080.0f), 0.5f, 500.0f);

	struct {
		glm::mat4 invProj;
		glm::mat4 proj;
	} pmatrix;
	pmatrix.invProj = glm::inverse(matrix.proj);
	pmatrix.proj = matrix.proj;

	struct {
		glm::mat4 proj;
		glm::mat4 view;
	} omatrix;
	omatrix.view = glm::mat4{ 1.0f };
	omatrix.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	oak::graphics::BufferInfo bufferInfo;
	//create matrix buffer
	bufferInfo.type = oak::graphics::BufferType::UNIFORM;
	bufferInfo.hint = oak::graphics::BufferHint::STREAM;
	bufferInfo.base = 0;
	auto& matrix_ubo = bufferHandle.add("matrix", oak::graphics::buffer::create(bufferInfo));
	//create proj invProj buffer
	bufferInfo.hint = oak::graphics::BufferHint::STATIC;
	bufferInfo.base = 2;
	auto& proj_ubo = bufferHandle.add("proj", oak::graphics::buffer::create(bufferInfo));
	oak::graphics::buffer::bind(proj_ubo);
	oak::graphics::buffer::data(proj_ubo, sizeof(pmatrix), &pmatrix);
	//create ortho matrix buffer
	bufferInfo.base = 1;
	auto& ortho_ubo = bufferHandle.add("ortho", oak::graphics::buffer::create(bufferInfo));
	oak::graphics::buffer::bind(ortho_ubo);
	oak::graphics::buffer::data(ortho_ubo, sizeof(omatrix), &omatrix);

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

	bufferInfo.hint = oak::graphics::BufferHint::STREAM;
	bufferInfo.base = 4;
	auto& light_ubo = bufferHandle.add("light", oak::graphics::buffer::create(bufferInfo));
	
	//shader setup
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "/res/shaders/deferred/geometry/vert.glsl";
	shaderInfo.fragment = "/res/shaders/deferred/geometry/frag.glsl";
	auto& sh_geometry = shaderHandle.add("geometry", oak::graphics::shader::create(shaderInfo));
	shaderInfo.vertex = "/res/shaders/forward/pass2d/vert.glsl";
	shaderInfo.fragment = "/res/shaders/forward/pass2d/frag.glsl";
	auto& sh_pass2d = shaderHandle.add("pass2d", oak::graphics::shader::create(shaderInfo));
	shaderInfo.vertex = "/res/shaders/forward/font/vert.glsl";
	shaderInfo.fragment = "/res/shaders/forward/font/frag.glsl";
	auto& sh_font = shaderHandle.add("font", oak::graphics::shader::create(shaderInfo));
	shaderInfo.vertex = "/res/shaders/deferred/particle/vert.glsl";
	shaderInfo.fragment = "/res/shaders/deferred/particle/frag.glsl";
	auto& sh_particle = shaderHandle.add("particle", oak::graphics::shader::create(shaderInfo));
	//textures
	oak::graphics::TextureInfo textureInfo;
	textureInfo.minFilter = oak::graphics::TextureFilter::NEAREST;
	auto& tex_character = textureHandle.add("character", oak::graphics::texture::create("/res/textures/character.png", textureInfo));
	textureInfo.minFilter = oak::graphics::TextureFilter::LINEAR;
	textureInfo.magFilter = oak::graphics::TextureFilter::LINEAR;
	auto& tex_font = textureHandle.add("font", oak::graphics::texture::create("/res/fonts/dejavu_sans/atlas.png", textureInfo));
	textureInfo.minFilter = oak::graphics::TextureFilter::LINEAR_MIP_NEAREST;
	textureInfo.magFilter = oak::graphics::TextureFilter::NEAREST;
	auto& tex_car = textureHandle.add("car", oak::graphics::texture::create("/res/textures/car.png", textureInfo));
	textureInfo.width = 4096;
	textureInfo.height = 4096;
	auto& colorAtlas = atlasHandle.add("color", 
		oak::graphics::texture::createAtlas({ 
			"/res/textures/pbr_rust/color.png",
			"/res/textures/pbr_grass/color.png",
			"/res/textures/pbr_rock/color.png",
		}, textureInfo));
	auto& normalAtlas = atlasHandle.add("normal",
		oak::graphics::texture::createAtlas({
			"/res/textures/pbr_rust/normal.png",
			"/res/textures/pbr_grass/normal.png"
		},	textureInfo));
	textureInfo.format = oak::graphics::TextureFormat::BYTE_R;
	auto& metalAtlas = atlasHandle.add("metal",
		oak::graphics::texture::createAtlas({
			"/res/textures/pbr_rust/metalness.png",
			"/res/textures/pbr_grass/metalness.png",
			"/res/textures/pbr_rock/metalness.png"
		}, textureInfo));
	auto& roughAtlas = atlasHandle.add("rough",
		oak::graphics::texture::createAtlas({
			"/res/textures/pbr_rust/roughness.png",
			"/res/textures/pbr_grass/roughness.png",
			"/res/textures/pbr_rock/roughness.png"
		}, textureInfo));

	//materials
	auto& mat_box = materialHandle.add("box", &sh_geometry, &colorAtlas.texture, &roughAtlas.texture, &metalAtlas.texture);
	auto& mat_car = materialHandle.add("car", &sh_geometry, &tex_car);
	auto& mat_overlay = materialHandle.add("overlay", &sh_pass2d, &tex_character);
	auto& mat_part = materialHandle.add("part", &sh_particle, &colorAtlas.texture, &roughAtlas.texture, &metalAtlas.texture);
	auto& mat_font = materialHandle.add("font", &sh_font, &tex_font);

	//meshes
	auto model_box = oak::graphics::loadModel("/res/models/box.obj");
	auto model_part = oak::graphics::loadModel("/res/models/bit.obj");
	auto model_car = oak::graphics::loadModel("/res/models/car.obj");
	oak::graphics::Mesh mesh_floor;

	mesh_floor.vertices = {
		oak::graphics::Mesh::Vertex{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		oak::graphics::Mesh::Vertex{ { 0.0f, 0.0f, 64.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		oak::graphics::Mesh::Vertex{ { 64.0f, 0.0f, 64.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
		oak::graphics::Mesh::Vertex{ { 64.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }
	};

	mesh_floor.indices = {
		0, 1, 2, 2, 3, 0
	};

	oak::graphics::Sprite spr_overlay{ 0.0f, 0.0f, 64.0f, 64.0f, { glm::vec2{ 0.0f }, glm::vec2{ 1.0f } } };

	oak::graphics::Font fnt_dejavu = oak::graphics::loadFont("/res/fonts/dejavu_sans/glyphs.fnt");

	//create entities
	oak::EntityId player = scene.createEntity();
	oak::addComponent<oak::PrefabComponent>(scene, player, std::hash<oak::string>{}("player"));
	oak::addComponent<TransformComponent>(scene, player);
	oak::addComponent<CameraComponent>(scene, player, glm::vec3{ 16.0f, 8.0f, 16.0f }, glm::vec3{ 0.0f, glm::pi<float>(), 0.0f });
	scene.activateEntity(player);

	oak::EntityId car = scene.createEntity();
	oak::addComponent<TransformComponent>(scene, car, glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 32.0f, 1.0f, 32.0f }));
	oak::addComponent<MeshComponent>(scene, car, &model_car[0], &mat_car);
	scene.activateEntity(car);

	oak::EntityId particle = scene.createEntity();
	oak::addComponent<oak::PrefabComponent>(scene, particle, std::hash<oak::string>{}("particle"));
	oak::addComponent<MeshComponent>(scene, particle, &model_part[0], &mat_part, colorAtlas.regions[0].second);
	scene.activateEntity(particle);

	oak::EntityId overlay = scene.createEntity();
	oak::addComponent<Transform2dComponent>(scene, overlay, glm::mat3{ 1.0f });
	oak::addComponent<SpriteComponent>(scene, overlay, &spr_overlay, &mat_overlay, 1u);
	//scene.activateEntity(overlay);

	oak::EntityId fps = scene.createEntity();
	oak::addComponent<Transform2dComponent>(scene, fps, glm::translate(glm::mat3{ 0.15f }, glm::vec2{ 8.0f }));
	oak::addComponent<TextComponent>(scene, fps, "fps: ", &fnt_dejavu, &mat_font, 1u);
	scene.activateEntity(fps);

	oak::Prefab fab_box{ "box", scene };
	fab_box.addComponent<TransformComponent>();
	fab_box.addComponent<MeshComponent>(&model_box[0], &mat_box, colorAtlas.regions[2].second, 0u);

	oak::EntityId floor = scene.createEntity();
	oak::addComponent<oak::PrefabComponent>(scene, floor, std::hash<oak::string>{}("floor"));
	oak::addComponent<TransformComponent>(scene, floor, glm::mat4{ 1.0f });
	oak::addComponent<MeshComponent>(scene, floor, &mesh_floor, &mat_box, colorAtlas.regions[1].second, 0u);
	scene.activateEntity(floor);

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	//main game loop
	isRunning = true;
	while (isRunning) {
		if (inputManager.getKey(oak::key::esc)) {
			if (inputManager.getCursorMode() == oak::CursorMode::NORMAL) {
				evtManager.getQueue<oak::CursorModeEvent>().emit({ oak::CursorMode::DISABLED });
			} else {
				evtManager.getQueue<oak::CursorModeEvent>().emit({ oak::CursorMode::NORMAL });
			}
			inputManager.setKey(oak::key::esc, 0);
		}
		inputManager.update();
		//create / destroy / activate / deactivate entities
		scene.update();
		//move camera
		cameraSystem.run();
		//update view
		auto& cc = oak::getComponent<const CameraComponent>(scene, player);
		matrix.view = glm::mat4{ glm::quat{ cc.rotation } };
		matrix.view[3] = glm::vec4{ cc.position, 1.0f };
		matrix.view = glm::inverse(matrix.view); //move view instead of camera
		//update lights
		for (int i = 0; i < 8; i++) {
			lights[i].pos = glm::vec3{ matrix.view * glm::vec4{ lpos[i], 1.0f} };
		}

		//upload buffers
		oak::graphics::buffer::bind(matrix_ubo);
		oak::graphics::buffer::data(matrix_ubo, sizeof(matrix), &matrix);
		oak::graphics::buffer::bind(light_ubo);
		oak::graphics::buffer::data(light_ubo, sizeof(lights), &lights);
	
		renderSystem.run();

		//check for exit
		if (!evtManager.getQueue<oak::WindowCloseEvent>().empty()) {
			isRunning = false; 
		}

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;

		oak::getComponent<TextComponent>(scene, fps).text = "fps: " + std::to_string(1.0f / dt.count());

		//do engine things
		evtManager.clear();
		oak::oalloc_frame.clear();
	}

	//clean up
	scene.reset();

	lfs.close();

	return 0;
}
