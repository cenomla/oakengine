#pragma once

#include "math.h"

namespace oak::graphics {
	struct Shader;
	struct ShaderInfo;
}

namespace oak::graphics::GLShader {

	Shader create(const ShaderInfo& info);
	void destroy(Shader& shader);

	void bind(const Shader& shader);
	void unbind();

	void setUniform(const Shader& shader, const char *name, const Mat4& value);
	void setUniform(const Shader& shader, const char *name, const Ivec2& value);
	void setUniform(const Shader& shader, const char *name, const Vec2& value);
	void setUniform(const Shader& shader, const char *name, const Vec3& value);
	void setUniform(const Shader& shader, const char *name, const Vec4& value);
	void setUniform(const Shader& shader, const char *name, unsigned int value);
	void setUniform(const Shader& shader, const char *name, int value);
	void setUniform(const Shader& shader, const char *name, float value);

}
