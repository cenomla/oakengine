#include "buffer.h"

#include "gl_api.h"

namespace oak::graphics {

	const TypeInfo Buffer::typeInfo = makeResourceInfo<Buffer>("buffer");

	void pup(Puper& puper, Buffer& data, const ObjInfo& info) {}

	void Buffer::destroy() {
		buffer::destroy(*this);
	}

}
