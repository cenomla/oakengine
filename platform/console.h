#pragma once

#include <system.h>
#include <scene.h>

class Console : public oak::System {
public:
	Console(oak::Scene *scene);

	void init() override;
	void terminate() override;
	void run() override;

private:
	oak::Scene *scene_;
	oak::EntityId console_;
};