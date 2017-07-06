#include <graphics/renderer.h>

class TestRenderer : public oak::graphics::Renderer {
public:
	void render(oak::graphics::Api *api) override;
	inline void setPipeline(oak::graphics::Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	oak::graphics::Pipeline *pipeline_;
};