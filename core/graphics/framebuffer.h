#pragma once

#include <cstddef>

#include "graphics/texture.h"
#include "container.h"

namespace oak::graphics {

	struct AttachmentInfo {
		const Texture *tex = nullptr;
		FramebufferAttachment attachment = FramebufferAttachment::COLOR0;
		int mipmap = 0;	
	};

	struct FramebufferInfo {
		oak::vector<AttachmentInfo> attachments;
		int width = 0, height = 0;
		FramebufferAttachment renderBuffer = FramebufferAttachment::DEPTH_STENCIL;
		bool useRenderbuffer = 0;
	};

	struct Framebuffer {
		uint32_t id = 0, rid = 0; //framebuffer id and renderbuffer id
		FramebufferInfo info;
	};

}