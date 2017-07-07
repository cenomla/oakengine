#pragma once

namespace oak::graphics {

	//graphical enums
	enum class TextureFormat {
		BYTE_RGBA,
		BYTE_RGB,
		FLOAT_RGBA,
		FLOAT_RGB,
		DEPTH_32F,
		DEPTH_32,
		DEPTH_24,
		DEPTH_16
	};

	enum class FramebufferAttachment {
		DEPTH_STENCIL = 33,
		DEPTH = 32,
		COLOR0 = 0,
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5,
		COLOR6,
		COLOR7,
		COLOR8,
		COLOR9,
		COLOR10,
		COLOR11,
		COLOR12,
		COLOR13,
		COLOR14,
		COLOR15,
		COLOR16
	};

	enum class AttributeType {
		POSITION,
		POSITION2D, 
		NORMAL,
		UV,
		COLOR
	};

	class Api {
	public:

		virtual void init() = 0;
		virtual void terminate() = 0;
		
		virtual void swap() = 0;
	};

}