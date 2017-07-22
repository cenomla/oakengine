#include "buffer.h"

#include "opengl/gl_api.h"

namespace oak::graphics {

	void Buffer::destroy() {
		buffer::destroy(*this);
	}

}