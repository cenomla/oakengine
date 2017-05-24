#pragma once

#include <functional>
#include <glm/glm.hpp>

#include <system.h>
#include <scene_utils.h>
#include <entity_cache.h>
#include <container.h>
#include <graphics/opengl/gl_renderer.h>
#include <graphics/opengl/gl_vertex_array.h>
#include <graphics/opengl/gl_material.h>
#include <graphics/opengl/gl_framebuffer.h>
#include <graphics/opengl/gl_buffer.h>
#include <graphics/batch.h>

class LightRenderer : public oak::System {
public:
	LightRenderer(oak::Scene& scene);

	void init() override;
	void run() override;

	void render(const oak::graphics::GLVertexArray& vao, const oak::graphics::Batch& batch);
private:
	struct Line {
		glm::vec2 start;
		glm::vec2 end;
	};

	oak::EntityCache shadowCache_;
	oak::EntityCache lightCache_;

	oak::graphics::GLVertexArray vao_;
	oak::graphics::GLBuffer vbo_;
	oak::graphics::GLTexture tex_;
	oak::graphics::GLFramebuffer fbo_;
	oak::graphics::GLShader shadowShader_;

	oak::graphics::GLVertexArray fvao_;
	oak::graphics::GLBuffer fvbo_;
	oak::graphics::GLShader fullscreenShader_;

	oak::vector<Line> lines_;

	oak::Scene *scene_;
};