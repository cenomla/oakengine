#include "shader.h"

#include "opengl/gl_api.h"

namespace oak::graphics {

	void Shader::destroy() {
		shader::destroy(*this);
	}

}