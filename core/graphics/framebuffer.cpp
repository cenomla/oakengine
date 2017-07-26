#include "framebuffer.h"

#include "gl_api.h"

namespace oak::graphics {

	void Framebuffer::destroy() {
		framebuffer::destroy(*this);
	}

}