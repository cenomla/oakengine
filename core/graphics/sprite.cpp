#include "sprite.h"

namespace oak::graphics {

	const TypeInfo Sprite::typeInfo = makeResourceInfo<Sprite>("sprite");

	void pup(Puper& puper, Sprite& data, const ObjInfo& info) {}

}
