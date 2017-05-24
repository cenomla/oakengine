#pragma once

#include "shader.h"
#include "texture.h"

namespace oak::graphics {

	struct Material {
		const Shader *shader;
		const Texture *texture;
	};

}