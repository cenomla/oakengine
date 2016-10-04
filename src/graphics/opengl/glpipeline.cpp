#include "glpipeline.h"

namespace oak::graphics {

	GLPipeline::GLPipeline() : 
		depthEnabled_{ GL_FALSE }, depthOp_{ GL_LESS }, 
		blendEnabled_{ GL_FALSE }, blendSrcRgb_{ GL_ZERO }, blendDstRgb_{ GL_ONE }, blendSrcAlpha_{ GL_SRC_ALPHA }, blendDstAlpha_{ GL_ONE_MINUS_SRC_ALPHA },
		blendOpRgb_{ GL_FUNC_ADD }, blendOpAlpha_{ GL_FUNC_ADD } {

	}

	void GLPipeline::bind() const {
		if (depthEnabled_ == GL_TRUE) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(depthOp_);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
		if (blendEnabled_ == GL_TRUE) {
			glEnable(GL_BLEND);
			glBlendFuncSeparate(blendSrcRgb_, blendDstRgb_, blendSrcAlpha_, blendDstAlpha_);
			glBlendEquationSeparate(blendOpRgb_, blendOpAlpha_);
		} else {
			glDisable(GL_BLEND);
		}
		glUseProgram(pid_);
	}

	void GLPipeline::unbind() const {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glUseProgram(0);
	}

	void GLPipeline::setDepth(GLenum enabled, GLenum op) {
		depthEnabled_ = enabled;
		depthOp_ = op;
	}

	void GLPipeline::setBlend(GLenum enabled, GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha, GLenum opRgb, GLenum opAlpha) {
		blendEnabled_ = enabled;
		blendSrcRgb_ = srcRgb;
		blendDstRgb_ = dstRgb;
		blendSrcAlpha_ = srcAlpha;
		blendDstAlpha_ = dstAlpha;
		blendOpRgb_ = opRgb;
		blendOpAlpha_ = opAlpha;
	}

	void GLPipeline::setProgram(GLuint pid) {
		pid_ = pid;
	}

}