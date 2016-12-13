#pragma once

#include <vector>
#include <graphics/opengl/gl_vertex_array.h>
#include <graphics/opengl/gl_buffer.h>
#include <graphics/opengl/gl_material.h>
#include <system.h>

#include "chunk.h"

class TileSystem : public oak::System {
public:
	TileSystem(oak::Engine &engine);

	void init() override;
	void render(const oak::graphics::GLMaterial &mat);

	Chunk& getChunk(int x, int y);
	const Chunk& getChunk(int x, int y) const;

private:
	std::vector<Chunk> chunks_;
	oak::graphics::GLVertexArray vao_;
	oak::graphics::GLBuffer vbo_;
	oak::graphics::GLBuffer ubo_;
};