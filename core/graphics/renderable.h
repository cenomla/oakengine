#pragma once

namespace oak::graphics {

	struct Renderable {
		virtual ~Renderable() {};
		virtual void draw(void *buffer, float x, float y, float rotation, float scale) const = 0;
		virtual size_t getMaterialId() const = 0;
		virtual size_t getVertexCount() const = 0;
	};

}