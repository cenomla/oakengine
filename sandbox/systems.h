#pragma once

#include <system.h>
#include <entity.h>

class SpriteSystem : public oak::System {
public:
	SpriteSystem(oak::Engine &engine);

	void init() override;
	void update();

private:
	oak::EntityCache cache_;
};

class TextSystem : public oak::System {
public:
	TextSystem(oak::Engine &engine);

	void init() override;
	void update();
	
private:
	oak::EntityCache cache_;
};

class PhysicsSystem : public oak::System {
public:
	PhysicsSystem(oak::Engine &engine);

	void init() override;
	void update(float dt);
};

class CollisionSystem : public oak::System {
public:
	CollisionSystem(oak::Engine &engine);

	void init() override;
	void update();

private:
	oak::EntityCache cache_;
};