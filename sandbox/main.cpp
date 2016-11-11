#include <iostream>
#include <fstream>
#include <chrono>

#include <cmath>

#include "graphics/opengl/gl_frame_renderer.h"
#include "graphics/opengl/gl_sprite_renderer.h"
#include <GLFW/glfw3.h>

#include "log.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"
#include "entity.h"
#include "prefab.h"
#include "lua_manager.h"
#include "components.h"
#include "resource_manager.h"
#include "event_queue.h"

std::chrono::duration<float> dt;

class SpriteSystem : public oak::System {
public:
	SpriteSystem(oak::Engine &engine) : oak::System{ engine, "sprite_system" } {
		for (size_t i = 0; i < 1024; i++) {
			keys_[i] = GLFW_RELEASE;
		}
	}

	void init() override {
		cache_.requireComponent<oak::TransformComponent>();
		cache_.requireComponent<oak::SpriteComponent>();
		engine_.getSystem<oak::EntityManager>().addCache(&cache_);
		engine_.getEventManager().add<oak::KeyEvent>(std::ref(*this));
		engine_.getTaskManager().addTask({ [this](){ update(); }, oak::Task::LOOP_BIT });
	}

	void update() {
		for (const auto& entity : cache_.entities()) {
			const auto &tc = entity.getComponent<oak::TransformComponent>();
			const auto &sc = entity.getComponent<oak::SpriteComponent>();
			auto &renderer = engine_.getSystem<oak::graphics::GLSpriteRenderer>();
			renderer.addSprite(tc.position, sc.animFrameX, sc.animFrameY, tc.scale, tc.rotationAngle, sc.sprite.get());
		}		
	}

	void operator()(const oak::KeyEvent &evt) {
		keys_[evt.key] = evt.action;
	}
private:
	oak::EntityCache cache_;
	int keys_[1024];
};

class PhysicsSystem : public oak::System {
public:
	struct ObjectInfo {
		glm::vec2 position;
		glm::vec2 extent;
	};

	PhysicsSystem(oak::Engine &engine) : oak::System{ engine, "physics_system" } {}

	void init() override {
		cache_.requireComponent<oak::TransformComponent>();
		cache_.requireComponent<oak::AABB2dComponent>();
		cache_.requireComponent<oak::PhysicsBody2dComponent>();
		engine_.getSystem<oak::EntityManager>().addCache(&cache_);
		engine_.getEventManager().add<oak::CollisionEvent>(std::ref(queue_));
		engine_.getTaskManager().addTask({ [this](){ 
			static float accum = 0.0f;
			accum += dt.count();
			while (accum >= 1.0f/60.0f) {
				update(1.0f/60.0f);
				accum -= 1.0f/60.0f; 
			}
		}, oak::Task::LOOP_BIT });
	}

	bool checkCollision(const ObjectInfo &a, const ObjectInfo &b, glm::vec2 &normal, float &depth) {
		glm::vec2 n = b.position - a.position;
		glm::vec2 overlap = a.extent + b.extent - glm::abs(n);

		if (overlap.x > 0 && overlap.y > 0) {
			if (overlap.x < overlap.y) {
				if (n.x < 0) {
					normal = glm::vec2{ -1, 0 };
				} else {
					normal = glm::vec2{ 1, 0 };
				}
				depth = overlap.x;
			} else {
				if (n.y < 0) {
					normal = glm::vec2{ 0, -1 };
				} else {
					normal = glm::vec2{ 0, 1 };
				}
				depth = overlap.y;
			}
			return true;
		}
		return false;
	}

	void update(float delta) {
		
		glm::vec2 normal;
		float depth;
		auto &entities = cache_.entities();

		for (auto &entity : entities) {
			auto& pc = entity.getComponent<oak::PhysicsBody2dComponent>();
			pc.force += glm::vec2{ 0.0f, 981.0f } * pc.mass;
			//integrate force
			pc.velocity += pc.force * pc.invMass * delta;
			pc.force = glm::vec2{ 0.0f };
		}

		for (size_t i = 0; i < entities.size(); i++) {
			const auto& entityA = entities[i];
			const auto& tcA = entityA.getComponent<oak::TransformComponent>();
			const auto& boxA = entityA.getComponent<oak::AABB2dComponent>();
			for (size_t j = i + 1; j < entities.size(); j++) {
				const auto& entityB = entities[j];
				const auto& tcB = entityB.getComponent<oak::TransformComponent>();
				const auto& boxB = entityB.getComponent<oak::AABB2dComponent>();
				if (checkCollision(
					{ glm::vec2{ tcA.position } + boxA.offset, boxA.halfExtent }, 
					{ glm::vec2{ tcB.position } + boxB.offset, boxB.halfExtent }, 
					normal, depth)) {
					engine_.getEventManager().emitEvent(oak::CollisionEvent{ entityA, entityB, normal, depth });
				}
			}
		}

		queue_.setCallback([this](const oak::CollisionEvent &evt){ resolve(evt); });
		for (size_t iter = 0; iter < 10; iter++) {
			queue_.processEvents();
		}
		//integrate physics
		for (auto& e : entities) {
			auto& tc = e.getComponent<oak::TransformComponent>();
			auto& pc = e.getComponent<oak::PhysicsBody2dComponent>();
			//move object by velocity
			tc.position += glm::vec3{ pc.velocity * delta, 0.0f };
		}

		queue_.setCallback([this](const oak::CollisionEvent &evt){ correctPosition(evt); });
		queue_.processEvents();

		queue_.clear();
	}

	void resolve(oak::CollisionEvent evt) {
		auto& pca = evt.entity.getComponent<oak::PhysicsBody2dComponent>();
		auto& pcb = evt.other.getComponent<oak::PhysicsBody2dComponent>();
		glm::vec2 rv = pcb.velocity - pca.velocity;

		float vn = glm::dot(rv, evt.normal);

		if (vn > 0) {
			return;
		}

		float e = glm::min(pca.restitution, pcb.restitution);

		float msum = pca.invMass + pcb.invMass;
		float j = (-(1 + e) * vn) / msum;

		glm::vec2 impulse = j * evt.normal;
		pca.velocity -= pca.invMass * impulse;
		pcb.velocity += pcb.invMass * impulse;

		//friction calculations
		rv = pcb.velocity - pca.velocity;
		glm::vec2 tangent = rv - glm::dot(rv, evt.normal) * evt.normal;
		if (glm::length(tangent) != 0) {
			tangent = glm::normalize(tangent);
		}

		float jt = -glm::dot(rv, tangent) / msum;

		float sf = std::sqrt(pca.staticFriction * pca.staticFriction + pcb.staticFriction * pcb.staticFriction);

		glm::vec2 fi{ 0.0f };
		if (glm::abs(jt) < j * sf) {
			fi = jt * tangent;
		} else {
			fi = -j * tangent * std::sqrt(pca.dynamicFriction * pca.dynamicFriction + pcb.dynamicFriction * pcb.dynamicFriction);
		}

		pca.velocity -= pca.invMass * fi;	
		pcb.velocity += pcb.invMass * fi;
	}

	void correctPosition(oak::CollisionEvent evt) {
		auto &tca = evt.entity.getComponent<oak::TransformComponent>();
		auto &pca = evt.entity.getComponent<oak::PhysicsBody2dComponent>();
		auto &tcb = evt.other.getComponent<oak::TransformComponent>();
		auto &pcb = evt.other.getComponent<oak::PhysicsBody2dComponent>();
		
		const float percent = 0.8f;
		const float slop = 0.01f;
		glm::vec2 correction = glm::max(evt.depth - slop, 0.0f) / (pca.invMass + pcb.invMass) * percent * evt.normal;
		tca.position -= glm::vec3{ pca.invMass * correction, 0.0f };
		tcb.position += glm::vec3{ pcb.invMass * correction, 0.0f };
		
	}

private:
	oak::EntityCache cache_;
	oak::EventQueue<oak::CollisionEvent> queue_;
};

int main(int argc, char** argv) {
	//setup log
	std::ofstream file{ "log" };
	//std::ostream log{ std::cout.rdbuf(&oak::log::cout_stream) };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(std::cout);

	//create the engine
	oak::Engine engine;
	engine.init();

	//create all the systems
	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::LuaManager luam{ engine };
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	oak::graphics::GLSpriteRenderer spriteRenderer{ engine };
	oak::ResourceManager resManager{ engine };
	oak::EntityManager entityManager{ engine };
	SpriteSystem spriteSystem{ engine };
	PhysicsSystem physicsSystem{ engine };

	//add a timer task
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	engine.getTaskManager().addTask({ [&lastFrame](){ 
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;
	}, oak::Task::LOOP_BIT });
	
	//add the systems to the engine and therefore initilize them
	engine.addSystem(&resManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);
	engine.addSystem(&luam);

	lua_State *L = luam.getState();
	engine.getTaskManager().addTask({ [L](){  
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		lua_pushstring(L, "update");
		lua_pushnumber(L, dt.count());
		oak::luah::call(L, 3, 0);

		oak::luah::getGlobal(L, "oak.es.process_events");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::call(L, 1, 0);
	}, oak::Task::LOOP_BIT });

	engine.addSystem(&physicsSystem);
	engine.addSystem(&spriteSystem);
	engine.addSystem(&spriteRenderer);

	//test event
	engine.getEventManager().add<oak::KeyEvent>([L](const oak::KeyEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		lua_pushstring(L, "key_press");
		lua_pushinteger(L, evt.key);
		lua_pushinteger(L, evt.scancode);
		lua_pushinteger(L, evt.action);
		lua_pushinteger(L, evt.mods);
		oak::luah::call(L, 6, 0);
	});
	engine.getEventManager().add<oak::ButtonEvent>([L](const oak::ButtonEvent &evt){
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		lua_pushstring(L, "button_press");
		lua_pushinteger(L, evt.button);
		lua_pushinteger(L, evt.action);
		lua_pushinteger(L, evt.mods);
		oak::luah::call(L, 5, 0);
	});
	engine.getEventManager().add<oak::MouseMoveEvent>([L](const oak::MouseMoveEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		lua_pushstring(L, "mouse_move");
		lua_pushinteger(L, evt.x);
		lua_pushinteger(L, evt.y);
		oak::luah::call(L, 4, 0);
	});

	//init some test stuff
	entityManager.addComponentHandle<oak::TransformComponent>();
	entityManager.addComponentHandle<oak::SpriteComponent>();
	entityManager.addComponentHandle<oak::AABB2dComponent>();
	entityManager.addComponentHandle<oak::PhysicsBody2dComponent>();

	auto &mat = resManager.add<oak::graphics::GLMaterial>("mat_player", std::hash<std::string>{}("mat_player"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat.create("core/graphics/shaders/pass2d", "res/textures/character");

	auto &mat0 = resManager.add<oak::graphics::GLMaterial>("mat_block", std::hash<std::string>{}("mat_block"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat0.create("core/graphics/shaders/pass2d", "res/textures/block");

	resManager.add<oak::graphics::Sprite>("spr_player", mat.id, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f);
	resManager.add<oak::graphics::Sprite>("spr_block", mat0.id, 128.0f, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f, 64.0f, 16.0f);

	auto& prefab = resManager.add<oak::Prefab>("player", &entityManager);
	prefab.addComponent<oak::TransformComponent>(glm::vec3{ 216.0f, 128.0f, 0.0f }, 1.0f, glm::vec3{0.0f}, 0.0f);
	prefab.addComponent<oak::SpriteComponent>(std::hash<std::string>{}("spr_player"), 0, 0);
	prefab.addComponent<oak::AABB2dComponent>(glm::vec2{ 8.0f, 8.0f }, glm::vec2{ 0.0f, 0.0f });
	prefab.addComponent<oak::PhysicsBody2dComponent>(glm::vec2{ 0.0f }, 0.025f * 0.2f, 0.4f, 0.2f, 0.1f);

	auto& prefab0 = resManager.add<oak::Prefab>("block", &entityManager);
	prefab0.addComponent<oak::SpriteComponent>(std::hash<std::string>{}("spr_block"), 0, 0);
	prefab0.addComponent<oak::TransformComponent>(glm::vec3{ 256.0f, 512.0f, 0.0f }, 2.0f, glm::vec3{ 0.0f }, 0.0f);
	prefab0.addComponent<oak::AABB2dComponent>(glm::vec2{ 128.0f, 32.0f }, glm::vec2{ 0.0f, 0.0f });
	prefab0.addComponent<oak::PhysicsBody2dComponent>(glm::vec2{ 0.0f }, 0.0f, 0.4f, 0.5f, 0.4f);

	oak::luah::loadScript(luam.getState(), "res/scripts/main.lua");

	//start ur engines
	engine.run();
	engine.destroy();

	return 0;
}