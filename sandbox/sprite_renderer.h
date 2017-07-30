#pragma once

#include "renderer.h"

class SpriteRenderer : public Renderer {
public:
	void init();
	void terminate();

	void render(oak::graphics::Api *api) override;
	inline void setPipeline(const Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	const Pipeline *pipeline_;
};