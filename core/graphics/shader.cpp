#include "shader.h"

#include "gl_api.h"

namespace oak::graphics {

	void Shader::destroy() {
		shader::destroy(*this);
	}

}