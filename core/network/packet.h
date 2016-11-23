#pragma once

namespace oak::network {


	struct Packet {
		static constexpr uint32_t ENTITY_CREATE = 1;
		static constexpr uint32_t ENTITY_REMOVE = 2;
		static constexpr uint32_t ENTITY_ACTIVATE = 3;
		static constexpr uint32_t ENTITY_DEACTIVATE = 4;
		static constexpr uint32_t COMPONENT_ADD = 5;
		static constexpr uint32_t COMPONENT_REMOVE = 6;
		static constexpr uint32_t COMPONENT_UPDATE = 7;
	};

}