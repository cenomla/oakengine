#include "update_events.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, TickEvent& data, const ObjInfo& info) {
	}

	void pup(Puper& puper, SimulateEvent& data, const ObjInfo& info) {
		pup(puper, data.dt, ObjInfo::make<float>(&info, "dt"));
	}

}