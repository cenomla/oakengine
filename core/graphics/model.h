#pragma once

#include "mesh.h"
#include "container.h"

namespace oak::graphics {

	oak::vector<Mesh> loadModel(const oak::string& path);

}