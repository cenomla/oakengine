#include <iostream>
#include <chrono>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/opengl/gl_api.h>

#include <graphics/shader.h>
#include <collision.h>
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

#include "render_system.h"
#include "poly_renderer.h"
#include "components.h"

struct stdoutstream : public oak::log::Stream {
	void write(const void *source, size_t size) override {
		fwrite(source, 1, size, stdout);
	}
};

bool isRunning = false;

struct CollisionSystem : public oak::System {

	void init() override {
		cache.requireComponent<TransformComponent>();
		cache.requireComponent<MeshComponent>();
		cache.requireComponent<ManifoldComponent>();
	}

	void run() override {
		cache.update(*scene);

		auto& ts = oak::getComponentStorage<TransformComponent>(*scene);
		auto& ms = oak::getComponentStorage<const MeshComponent>(*scene);
		auto& cs = oak::getComponentStorage<ManifoldComponent>(*scene);

		auto& entities = cache.entities();

		auto& A = entities[0];
		auto& Atc = oak::getComponent<TransformComponent>(ts, A);
		auto& Amc = oak::getComponent<const MeshComponent>(ms, A);
		auto& Acc = oak::getComponent<ManifoldComponent>(cs, A);
		for (size_t i = 1; i < entities.size(); i++) {
			
			auto& B = entities[i];
			auto& Btc = oak::getComponent<const TransformComponent>(ts, B);
			auto& Bmc = oak::getComponent<const MeshComponent>(ms, B);
			auto& Bcc = oak::getComponent<ManifoldComponent>(cs, B);
			
			oak::Simplex splex{ &oak::oalloc_frame };
			auto collides = oak::gjk(*Amc.mesh, Atc.transform, *Bmc.mesh, Btc.transform, splex);
			Acc.collides = Acc.collides ? 1 : collides;
			Bcc.collides = Bcc.collides ? 1 : collides;
			if (collides) {
				glm::vec2 N;
				float d;
				oak::epa(*Amc.mesh, Atc.transform, *Bmc.mesh, Btc.transform, splex, N, d);
				Atc.transform[2] += glm::vec3{ -N * d, 0.0f };
			}
		}
	}

	oak::EntityCache cache;
	oak::Scene *scene;
};

int main(int argc, char** argv) {
	//setup log
	stdoutstream sos;
	oak::log::cout.addStream(&sos);
	oak::log::cwarn.addStream(&sos);
	oak::log::cerr.addStream(&sos);

	//init engine managers
	oak::EventManager evtManager;
	oak::InputManager inputManager;
	oak::FileManager fileManager;
	oak::SystemManager sysManager;
	oak::ComponentTypeManager chs;
	oak::ResourceManager resManager;

	inputManager.bind("move_up", oak::key::w, true);
	inputManager.bind("move_down", oak::key::s, true);
	inputManager.bind("move_left", oak::key::a, true);
	inputManager.bind("move_right", oak::key::d, true);

	fileManager.mount("{$cwd}/platform/res", "/res");
	fileManager.mount("{$cwd}/platform", "/res/shaders");
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
	auto& shaderHandle = resManager.get<oak::graphics::Shader>();
	auto& materialHandle = resManager.get<oak::graphics::Material>();

	//create the scene
	oak::Scene scene;

	//create all the systems
	oak::graphics::GLApi api;

	//create the rendering system
	RenderSystem renderSystem{ &scene, &api };
	PolyRenderer polyRenderer;
	renderSystem.pushLayerBack(&polyRenderer);
	CollisionSystem collisionSystem;
	collisionSystem.scene = &scene;

	//add them to the system manager
	sysManager.addSystem(&renderSystem, "render_system");
	sysManager.addSystem(&collisionSystem, "collision_system");

	//create component type handles
	chs.addHandle<oak::EventComponent>("event");
	chs.addHandle<oak::PrefabComponent>("prefab");
	chs.addHandle<TransformComponent>("transform");
	chs.addHandle<MeshComponent>("mesh");
	chs.addHandle<ManifoldComponent>("manifold");

	//create component storage
	oak::ComponentStorage eventStorage{ "event" };
	oak::ComponentStorage prefabStorage{ "prefab" };
	oak::ComponentStorage transformStorage{ "transform" };
	oak::ComponentStorage meshStorage{ "mesh" };
	oak::ComponentStorage manifoldStorage{ "manifold" };

	//add component storage to scene
	scene.addComponentStorage(&eventStorage);
	scene.addComponentStorage(&prefabStorage);
	scene.addComponentStorage(&transformStorage);
	scene.addComponentStorage(&meshStorage);
	scene.addComponentStorage(&manifoldStorage);

	//init the test renderer
	polyRenderer.init();

	//setup uniforms
	struct {
		glm::mat4 proj;
		glm::mat4 view;
	} omatrix;
	omatrix.view = glm::mat4{ 1.0f };
	omatrix.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	oak::graphics::BufferInfo bufferInfo;
	//create ortho matrix buffer
	bufferInfo.type = oak::graphics::BufferType::UNIFORM;
	bufferInfo.hint = oak::graphics::BufferHint::STATIC;
	bufferInfo.base = 1;
	auto& ortho_ubo = bufferHandle.add("ortho", oak::graphics::buffer::create(bufferInfo));
	oak::graphics::buffer::bind(ortho_ubo);
	oak::graphics::buffer::data(ortho_ubo, sizeof(omatrix), &omatrix);

	oak::Mesh2d m1;
	m1.vertices = oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { 0.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 0.0f, 32.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 32.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 0.0f }, glm::vec2{ 1.0f } },
	};

	oak::Mesh2d m2;
	m2.vertices = oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { 0.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 16.0f, 16.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 0.0f }, glm::vec2{ 1.0f } },
	};

	oak::Mesh2d m3;
	m3.vertices = oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { -8.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { -12.0f, 16.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { -4.0f, 20.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 8.0f, 4.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 6.0f, 0.0f }, glm::vec2{ 1.0f } },
	};

	//create entities
	oak::Prefab polygon{ "polygon", scene };
	polygon.addComponent<TransformComponent>(glm::rotate(glm::mat3{ 1.0f }, 45.0f));
	polygon.addComponent<MeshComponent>(&m2);
	polygon.addComponent<ManifoldComponent>();

	oak::EntityId p1 = polygon.createInstance();
	scene.activateEntity(p1);

	oak::EntityId p2 = polygon.createInstance();
	oak::getComponent<TransformComponent>(scene, p2).transform = glm::translate(glm::mat3{ 1.0f }, glm::vec2{ 64.0f });
	oak::getComponent<MeshComponent>(scene, p2).mesh = &m2;
	scene.activateEntity(p2);

	oak::EntityId p3 = polygon.createInstance();
	oak::getComponent<TransformComponent>(scene, p3).transform = glm::translate(glm::mat3{ 1.0f }, glm::vec2{ 256.0f });
	oak::getComponent<MeshComponent>(scene, p3).mesh = &m3;
	scene.activateEntity(p3);

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	//main game loop
	isRunning = true;
	while (isRunning) {
		inputManager.update();
		//create / destroy / activate / deactivate entities
		scene.update();

		if (inputManager.getKey(oak::key::w)) {
			oak::getComponent<TransformComponent>(scene, p1).transform[2] += glm::vec3{ 0.0f, -2.0f, 0.0f };
		}

		if (inputManager.getKey(oak::key::a)) {
			oak::getComponent<TransformComponent>(scene, p1).transform[2] += glm::vec3{ -2.0f, 0.0f, 0.0f };
		}

		if (inputManager.getKey(oak::key::s)) {
			oak::getComponent<TransformComponent>(scene, p1).transform[2] += glm::vec3{ 0.0f, 2.0f, 0.0f };
		}

		if (inputManager.getKey(oak::key::d)) {
			oak::getComponent<TransformComponent>(scene, p1).transform[2] += glm::vec3{ 2.0f, 0.0f, 0.0f };
		}

		collisionSystem.run();
	
		renderSystem.run();

		//check for exit
		if (!evtManager.getQueue<oak::WindowCloseEvent>().empty()) {
			isRunning = false; 
		}

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;

		//do engine things
		evtManager.clear();
		oak::oalloc_frame.clear();
	}

	//clean up
	polyRenderer.terminate();
	scene.reset();

	return 0;
}
