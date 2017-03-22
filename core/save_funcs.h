#pragma once

#include "save_manager.h"

namespace oak {

	class Entity;

	namespace save {

		void streamEntity(SaveManager& sm, Entity& data);
	}

}