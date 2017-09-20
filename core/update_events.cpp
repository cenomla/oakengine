#include "update_events.h"

#include "pup.h"

namespace oak {

	const TypeInfo TickEvent::typeInfo = makeEventInfo<TickEvent>("tick");
	const TypeInfo SimulateEvent::typeInfo = makeEventInfo<SimulateEvent>("simulate");

	void pup(Puper& puper, TickEvent& data, const ObjInfo& info) {
	}

	void pup(Puper& puper, SimulateEvent& data, const ObjInfo& info) {
		pup(puper, data.dt, ObjInfo::make<float>(&info, "dt"));
	}

}
