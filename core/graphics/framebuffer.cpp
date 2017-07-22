#include "framebuffer.h"

#include "opengl/gl_api.h"

namespace oak::graphics {

	void Framebuffer::destroy() {
		framebuffer::destroy(*this);
	}

}