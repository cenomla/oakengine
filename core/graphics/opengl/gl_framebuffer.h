#pragma once

namespace oak::graphics {
	struct Framebuffer;
	struct FramebufferInfo;
}

namespace oak::graphics::GLFramebuffer {

		Framebuffer create(const FramebufferInfo& info);
		void destroy(Framebuffer& framebuffer);

		void bind(const Framebuffer& framebuffer);
		void unbind();

}
