#pragma once

#include <system.h>
#include <entity_cache.h>

class SpriteSystem : public oak::System {
public:
	SpriteSystem(oak::Scene& scene);

	void init() override;
	void run() override;

private:
	oak::EntityCache cache_;
	oak::Scene *scene_;
};

class TextSystem : public oak::System {
public:
	TextSystem(oak::Scene& scene);

	void init() override;
	void run() override;
	
private:
	oak::EntityCache cache_;
	oak::Scene *scene_;
};
