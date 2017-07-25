#pragma once

#include "shader.h"
#include "texture.h"
#include "attribute_layout.h"

namespace oak::graphics {

	struct Material {
		const Shader *shader;
		const Texture *textures[16] = { nullptr };
	};

}