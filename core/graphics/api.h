#pragma once

#include "math.h"

namespace oak::graphics {

	struct Batch;
	struct Material;

	//graphical enums
	enum class TextureFormat {
		BYTE_RGBA,
		BYTE_RGB,
		BYTE_R,
		FLOAT_RGBA,
		FLOAT_RGB,
		FLOAT_R,
		DEPTH_32F,
		DEPTH_32,
		DEPTH_24,
		DEPTH_16
	};

	enum class TextureType {
		IMAGE_2D,
		IMAGE_3D,
		ARRAY_2D,
		CUBEMAP
	};

	enum class TextureFilter {
		NEAREST,
		LINEAR,
		LINEAR_MIP_LINEAR,
		NEAREST_MIP_NEAREST,
		LINEAR_MIP_NEAREST,
		NEAREST_MIP_LINEAR,
	};

	enum class TextureWrap {
		REPEAT,
		CLAMP_EDGE
	};

	enum class BufferType {
		VERTEX,
		ELEMENT,
		UNIFORM,
		PIXEL_PACK,
		PIXEL_UNPACK
	};

	enum class BufferHint {
		STREAM,
		STATIC
	};

	enum class BufferAccess {
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE
	};

	enum class PixelReadFormat {
		DEPTH,
		COLOR_RGB,
		COLOR_RGBA
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
		COLOR,
		INSTANCE_TRANSFORM,
		INSTANCE_POSITION,
		INSTANCE_POSITION2D,
		INSTANCE_NORMAL,
		INSTANCE_UV,
		INSTANCE_COLOR
	};

	enum class BoolOp {
		NONE = -1,
		NEVER,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS
	};

	enum class BlendMode {
		NONE,
		NORMAL,
		ADD,
		MULTIPLY
	};

	enum class FaceCull {
		NONE = -1,
		FRONT,
		BACK
	};

	using View = Ivec4;

	struct MaskState {
		bool red = true, green = true, blue = true, alpha = true, depth = true, stencil = true;
	};

	class Api {
	public:

		struct State {
			Vec4 clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
			float clearDepth = 1.0f;
			BoolOp depthOp = BoolOp::NONE;
			View scissorRect;
			MaskState drawMask;
			FaceCull faceCull{ FaceCull::BACK };
			View viewport;
			BlendMode blendMode = BlendMode::NONE;
		};

		virtual ~Api() {};

		virtual void init() = 0;
		virtual void terminate() = 0;
		
		virtual void setState(const State& state) = 0;
		virtual const State& getState() const = 0;

		virtual void clear(bool color = false, bool depth = false, bool stencil = false) = 0;
		virtual void draw(const Batch& batch) = 0;
		virtual void drawFullscreen(const Material& material) = 0;
		virtual void readPixels(int x, int y, int width, int height, PixelReadFormat format, void *offset) = 0;
		virtual void swap() = 0;
	};

}
