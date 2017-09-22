#include "material.h"

namespace oak::graphics {

	const TypeInfo Material::typeInfo = makeResourceInfo<Material>("materia");

	void pup(Puper& puper, Material& data, const ObjInfo& info) { }

}
