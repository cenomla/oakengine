#pragma once

#include <glad/glad.h>

namespace oak::graphics {

	class GLPipeline {
	public:
		GLPipeline();

		void bind() const;
		void unbind() const;

		void setDepth(GLenum enabled, GLenum op);
		void setBlend(GLenum enabled, GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha, GLenum opRgb, GLenum opAlpha);
		void setProgram(GLuint pid);
	private:
		GLenum depthEnabled_, depthOp_;
		GLenum blendEnabled_, blendSrcRgb_, blendDstRgb_, blendSrcAlpha_, blendDstAlpha_, blendOpRgb_, blendOpAlpha_;
		GLuint pid_;
	};

}