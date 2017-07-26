#include "buffer.h"

#include "gl_api.h"

namespace oak::graphics {

	void Buffer::destroy() {
		buffer::destroy(*this);
	}

}