#include "shader.h"

#include "gl_api.h"

namespace oak::graphics {

	const TypeInfo Shader::typeInfo = makeResourceInfo<Shader>("shader");

	void pup(Puper& puper, Shader& data, const ObjInfo& info) {}

	void Shader::destroy() {
		shader::destroy(*this);
	}

}
