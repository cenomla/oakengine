#include "components.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, TransformComponent& comp, const ObjInfo& info) {
		pup(puper, comp.transform, info + ObjInfo{ "mesh" });
	}

	void pup(Puper& puper, MeshComponent& comp, const ObjInfo& info) {
		pup(puper, comp.mesh, info + ObjInfo{ "mesh" });
	}

	void pup(Puper& puper, PositionComponent& comp, const ObjInfo& info) {
		pup(puper, comp.position, info + ObjInfo{ "position" });
		pup(puper, comp.scale, info + ObjInfo{ "scale" });
		pup(puper, comp.rotationAxis, info + ObjInfo{ "rotation_axis" });
		pup(puper, comp.rotationAngle, info + ObjInfo{ "rotation_angle" });
	}

	void pup(Puper& puper, TextComponent& comp, const ObjInfo& info) {
		pup(puper, comp.font, info + ObjInfo{ "font" });
		pup(puper, comp.text, info + ObjInfo{ "text" });
		pup(puper, comp.offset, info + ObjInfo{ "offset" });
		pup(puper, comp.layer, info + ObjInfo{ "layer" });
		pup(puper, comp.size, info + ObjInfo{ "size" });
	}

	void pup(Puper& puper, AABB2dComponent& comp, const ObjInfo& info) {
		pup(puper, comp.halfExtent, info + ObjInfo{ "half_extent" });
		pup(puper, comp.offset, info + ObjInfo{ "offset" });
	}

	void pup(Puper& puper, SpriteComponent& comp, const ObjInfo& info) {
		pup(puper, comp.sprite, info + ObjInfo{ "sprite" });
		pup(puper, comp.scale, info + ObjInfo{ "scale" });
		pup(puper, comp.animFrameX, info + ObjInfo{ "animframe_x" });
		pup(puper, comp.animFrameY, info + ObjInfo{ "animframe_y" });
		pup(puper, comp.layer, info + ObjInfo{ "layer" });
	}

	void pup(Puper& puper, PhysicsBody2dComponent& comp, const ObjInfo& info) {
		pup(puper, comp.velocity, info + ObjInfo{ "velocity" });
		pup(puper, comp.force, info + ObjInfo{ "force" });
		pup(puper, comp.mass, info + ObjInfo{ "mass" });
		pup(puper, comp.invMass, info + ObjInfo{ "inv_mass" });
		pup(puper, comp.restitution, info + ObjInfo{ "restitution" });
		pup(puper, comp.staticFriction, info + ObjInfo{ "static_friction" });
		pup(puper, comp.dynamicFriction, info + ObjInfo{ "dynamic_friction" });
	}

}