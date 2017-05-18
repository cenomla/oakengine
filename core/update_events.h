#pragma once

namespace oak {

	class Puper;
	struct ObjInfo;

	struct TickEvent {};

	struct SimulateEvent {
		float dt;
	};

	void pup(Puper& puper, TickEvent& data, const ObjInfo& info);
	void pup(Puper& puper, SimulateEvent& data, const ObjInfo& info);

}