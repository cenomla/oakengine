#include <iostream>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/gl_api.h>
#include <graphics/camera.h>
#include <util/file_buffer.h>
#include <collision.h>
#include <log.h>
#include <file_manager.h>
#include <system_manager.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_manager.h>
#include <audio_manager.h>
#include <component_storage.h>
#include <core_components.h>
#include <input_events.h>
#include <scene_events.h>
#include <update_events.h>
#include <input.h>
#include <prefab.h>
#include <scene_utils.h>

#include "render_system.h"
#include "components.h"
#include "console.h"

bool isRunning = false;

struct CollisionSystem : public oak::System {

	struct Manifold {
		oak::EntityId A;
		oak::EntityId B;
		glm::vec2 N;
		float d;
	};

	void doCollision() {
		auto& ts = oak::getComponentStorage<TransformComponent>(*scene);
		auto& ms = oak::getComponentStorage<const MeshComponent>(*scene);

		auto& entities = collisionCache.entities();

		oak::Simplex splex{ &oak::oalloc_frame };
		for (size_t i = 0; i < entities.size(); i++) {
			auto& A = entities[i];
			auto& Atc = oak::getComponent<TransformComponent>(A, ts);
			auto& Amc = oak::getComponent<const MeshComponent>(A, ms);
			for (size_t j = i + 1; j < entities.size(); j++) {
				
				auto& B = entities[j];
				auto& Btc = oak::getComponent<TransformComponent>(B, ts);
				auto& Bmc = oak::getComponent<const MeshComponent>(B, ms);
				
				auto collides = oak::gjk(*Amc.mesh, Atc.transform, *Bmc.mesh, Btc.transform, splex);
				if (collides) {
					//calculate penetration vector
					glm::vec2 N;
					float d;
					oak::epa(*Amc.mesh, Atc.transform, *Bmc.mesh, Btc.transform, splex, N, d);
					//add collision data to manifold list
					manifolds_.push_back({ A, B, N, d });
				}
				splex.clear();
			}
		}
	}

	void doPhysics() {
		auto& ts = oak::getComponentStorage<TransformComponent>(*scene);
		auto& vs = oak::getComponentStorage<VelocityComponent>(*scene);
		auto& rbs = oak::getComponentStorage<const RigidBodyComponent>(*scene);
		for (auto& manifold : manifolds_) {
			if (!rigidBodyCache.contains(manifold.A) || !rigidBodyCache.contains(manifold.B)) { continue; }
			//get components
			auto [tcA, vcA, rbA] = oak::getComponents<TransformComponent, VelocityComponent, const RigidBodyComponent>(manifold.A, ts, vs, rbs);
			auto [tcB, vcB, rbB] = oak::getComponents<TransformComponent, VelocityComponent, const RigidBodyComponent>(manifold.B, ts, vs, rbs);

			// auto& tcA = oak::getComponent<TransformComponent>(ts, manifold.A);
			// auto& tcB = oak::getComponent<TransformComponent>(ts, manifold.B);
			// auto& vcA = oak::getComponent<VelocityComponent>(vs, manifold.A);
			// auto& vcB = oak::getComponent<VelocityComponent>(vs, manifold.B);
			// auto& rbA = oak::getComponent<const RigidBodyComponent>(rbs, manifold.A);
			// auto& rbB = oak::getComponent<const RigidBodyComponent>(rbs, manifold.B);

			glm::vec2 rv = vcB.velocity - vcA.velocity;
			float vN = glm::dot(rv, manifold.N);

			if (vN > 0.0f) {
				continue; //do not solve velocities are separating
			}
			//integrate impulse
			float e = glm::min(rbA.restitution, rbB.restitution);
			float ms = rbA.invMass + rbB.invMass;

			if (ms < 0.0000001f) { continue; }

			float j = -(1.0f + e) * vN;
			j /= ms;

			glm::vec2 imp = j * manifold.N;
			vcA.velocity -= rbA.invMass * imp;
			vcB.velocity += rbB.invMass * imp;

			//positional correct
			const float percent = 0.5f;
			const float slope = 0.01f;
			glm::vec2 correction = glm::max(manifold.d - slope, 0.0f) / (ms) * percent * manifold.N;
			tcA.transform[2] -= glm::vec3{ rbA.invMass * correction, 0.0f };
			tcB.transform[2] += glm::vec3{ rbB.invMass * correction, 0.0f };
		}
		//integrate velocity
		for (auto& entity : rigidBodyCache.entities()) {
			auto [tc, vc] = oak::getComponents<TransformComponent, VelocityComponent>(entity, ts, vs);
			
			tc.transform[2] += glm::vec3{ vc.velocity * dt + (0.5f * vc.acceleration * dt * dt), 0.0f };
			vc.velocity += vc.acceleration * dt;	
		}
	}

	void movePlayer() {
		auto& vc = oak::getComponent<VelocityComponent>(playerCache.entities()[0], *scene);
		if (oak::InputManager::inst().getAction("move_up") == oak::action::pressed) {
			vc.velocity.y -= 244.0f;
			oak::InputManager::inst().setAction("move_up", oak::action::released);
		}

		if (oak::InputManager::inst().getAction("move_left")) {
			vc.velocity.x = -200.0f;
		} else if (oak::InputManager::inst().getAction("move_right")) {
			vc.velocity.x = 200.0f;
		} else {
			vc.velocity.x = 0.0f;
		}
	}

	void init() override {
		collisionCache.requireComponent<TransformComponent>();
		collisionCache.requireComponent<MeshComponent>();
		playerCache.requirePrefab(std::hash<oak::string>{}("player"));
		rigidBodyCache.requireComponent<TransformComponent>();
		rigidBodyCache.requireComponent<VelocityComponent>();
		rigidBodyCache.requireComponent<RigidBodyComponent>();
	}

	void run() override {
		collisionCache.update(*scene);
		playerCache.update(*scene);
		rigidBodyCache.update(*scene);
		movePlayer();
		doCollision();
		doPhysics();
		manifolds_.clear();
	}

	oak::EntityCache collisionCache;
	oak::EntityCache playerCache;
	oak::EntityCache rigidBodyCache;
	oak::Scene *scene;
	oak::vector<Manifold> manifolds_;
	float dt;
};

int main(int argc, char** argv) {
	//setup log
	oak::FileBuffer fb{ stdout };
	oak::Stream stream{ &fb };
	oak::log::cout.addStream(&stream);
	oak::log::cwarn.addStream(&stream);
	oak::log::cerr.addStream(&stream);

	//init engine managers
	oak::EventManager evtManager;
	oak::InputManager inputManager;
	oak::AudioManager audioManager;
	oak::FileManager fileManager;
	oak::SystemManager sysManager;
	oak::ComponentTypeManager chs;
	oak::ResourceManager resManager;

	inputManager.bind("move_up", oak::key::w, true);
	inputManager.bind("move_down", oak::key::s, true);
	inputManager.bind("move_left", oak::key::a, true);
	inputManager.bind("move_right", oak::key::d, true);

	//fileManager.mount("{$cwd}/platform/res", "/res");
	fileManager.mount("{$installDir}/platform", "/res");
	fileManager.mount("{$installDir}/platform", "/res/shaders");
	fileManager.mount("{$installDir}/core/graphics/shaders", "/res/shaders");
	fileManager.mount("{$installDir}/core/graphics/shaders/forward", "/res/shaders");
	

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

	//create component type handles
	chs.addHandle<oak::EventComponent>("event");
	chs.addHandle<oak::PrefabComponent>("prefab");
	chs.addHandle<TransformComponent>("transform");
	chs.addHandle<VelocityComponent>("velocity");
	chs.addHandle<MeshComponent>("mesh");
	chs.addHandle<RigidBodyComponent>("rigid_body");
	chs.addHandle<SpriteComponent>("sprite");
	chs.addHandle<TextComponent>("text");

	//create component storage
	oak::ComponentStorage eventStorage{ chs.getHandle("event") };
	oak::ComponentStorage prefabStorage{ chs.getHandle("prefab") };
	oak::ComponentStorage transformStorage{ chs.getHandle("transform") };
	oak::ComponentStorage velocityStorage{ chs.getHandle("velocity") };
	oak::ComponentStorage meshStorage{ chs.getHandle("mesh") };
	oak::ComponentStorage massStorage{ chs.getHandle("rigid_body") };
	oak::ComponentStorage spriteStorage{ chs.getHandle("sprite") };
	oak::ComponentStorage textStorage{ chs.getHandle("text") };

	//get references to resource storage containers
	auto& bufferHandle = resManager.get<oak::graphics::Buffer>();
	auto& textureHandle = resManager.get<oak::graphics::Texture>();
	auto& shaderHandle = resManager.get<oak::graphics::Shader>();
	auto& materialHandle = resManager.get<oak::graphics::Material>();
	auto& meshHandle = resManager.get<oak::Mesh2d>();
	auto& audioHandle = resManager.get<oak::AudioObject>();

	//create the scene
	oak::Scene scene;
	//add component storage to scene
	scene.addComponentStorage(&eventStorage);
	scene.addComponentStorage(&prefabStorage);
	scene.addComponentStorage(&transformStorage);
	scene.addComponentStorage(&velocityStorage);
	scene.addComponentStorage(&meshStorage);
	scene.addComponentStorage(&massStorage);
	scene.addComponentStorage(&spriteStorage);
	scene.addComponentStorage(&textStorage);

	//create all the systems
	oak::graphics::GLApi api;

	//create the rendering system
	RenderSystem renderSystem{ &scene, &api };
	CollisionSystem collisionSystem;
	collisionSystem.scene = &scene;
	Console console{ &scene };

	//add them to the system manager
	sysManager.addSystem(&renderSystem, "render_system");
	sysManager.addSystem(&collisionSystem, "collision_system");
	sysManager.addSystem(&console, "console");

	//load audio file
	auto& snd_chip = audioHandle.add("chip", audioManager.createSound("/res/chip.ogg"));
	audioHandle.add("test", audioManager.createSound("/res/test.ogg"));

	//setup uniforms
	oak::graphics::Camera camera;
	camera.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	oak::graphics::BufferInfo bufferInfo;
	//create ortho matrix buffer
	bufferInfo.type = oak::graphics::BufferType::UNIFORM;
	bufferInfo.hint = oak::graphics::BufferHint::STATIC;
	bufferInfo.base = 1;
	auto& ortho_ubo = bufferHandle.add("ortho", oak::graphics::buffer::create(bufferInfo));
	oak::graphics::buffer::bind(ortho_ubo);
	oak::graphics::buffer::data(ortho_ubo, sizeof(camera), &camera);

	meshHandle.add("m1", oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { 0.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 0.0f, 32.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 32.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 0.0f }, glm::vec2{ 1.0f } },
	});

	meshHandle.add("m2", oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { 0.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 16.0f, 16.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 32.0f, 0.0f }, glm::vec2{ 1.0f } },
	});

	meshHandle.add("m3", oak::vector<oak::Mesh2d::Vertex>{
		oak::Mesh2d::Vertex{ { -8.0f, 0.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { -12.0f, 16.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { -4.0f, 20.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 8.0f, 4.0f }, glm::vec2{ 1.0f } },
		oak::Mesh2d::Vertex{ { 6.0f, 0.0f }, glm::vec2{ 1.0f } },
	});

	//create entities
	oak::Prefab player{ "player", scene };
	player.addComponent<TransformComponent>(glm::mat3{ 1.0f });
	player.addComponent<VelocityComponent>(glm::vec2{ 0.0f }, glm::vec2{ 0.0f, 488.0f });
	player.addComponent<MeshComponent>(oak::Resource<oak::Mesh2d>{ "m1" });
	player.addComponent<RigidBodyComponent>(0.0f, 20.0f, 1.0f / 20.0f);

	scene.activateEntity(player.createInstance());

	oak::Prefab polygon{ "polygon", scene };
	polygon.addComponent<TransformComponent>(glm::mat3{ 1.0f });
	polygon.addComponent<VelocityComponent>();
	polygon.addComponent<MeshComponent>(oak::Resource<oak::Mesh2d>{ "m1" });
	polygon.addComponent<RigidBodyComponent>(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 6; i++) {
		oak::EntityId e = polygon.createInstance();
		oak::getComponent<TransformComponent>(e, scene).transform = glm::translate(glm::mat3{ 1.0f }, glm::vec2{ 64.0f + i * 45.0f, 512.0f }) * glm::rotate(glm::mat3{ 1.0f }, glm::radians(45.0f));
		scene.activateEntity(e);
	}

	oak::EntityId p2 = polygon.createInstance();
	oak::getComponent<TransformComponent>(p2, scene).transform = glm::translate(glm::mat3{ 1.0f }, glm::vec2{ 64.0f });
	oak::getComponent<MeshComponent>(p2, scene).mesh = { "m2" };
	scene.activateEntity(p2);

	oak::EntityId p3 = polygon.createInstance();
	oak::getComponent<TransformComponent>(p3, scene).transform = glm::translate(glm::mat3{ 1.0f }, glm::vec2{ 256.0f });
	oak::getComponent<MeshComponent>(p3, scene).mesh = { "m3" };
	scene.activateEntity(p3);

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	//main game loop
	isRunning = true;
	while (isRunning) {
		inputManager.update();
		audioManager.update();
		//create / destroy / activate / deactivate entities
		scene.update();

		console.run();

		collisionSystem.dt = dt.count();
		collisionSystem.run();
	
		renderSystem.run();

		for (auto& evt : evtManager.getQueue<oak::KeyEvent>()) {
			if (evt.key == oak::key::p && evt.action == oak::action::released) {
				audioManager.playSound(snd_chip, 0, 1.0f);
			}
		}

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
	scene.reset();

	return 0;
}
