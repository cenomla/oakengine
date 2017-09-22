#pragma once

#include "shader.h"
#include "texture.h"
#include "attribute_layout.h"
#include "resource.h"

namespace oak::graphics {

	struct Material {
		static const TypeInfo typeInfo;

		const Shader *shader;
		const Texture *textures[16] = { nullptr };
	};

	void pup(Puper& puper, Material& data, const ObjInfo& info);

}
