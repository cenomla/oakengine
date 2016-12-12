#include "components.h"

namespace oak::util {

	void pup(Puper &puper, PrefabComponent &comp, const ObjInfo &info) {
		pup(puper, comp.prefab, info);
		//pup std::bitset
		uint64_t value = comp.ownsMask.to_ullong();
		pup(puper, value, info);
		comp.ownsMask = { value };
	}

	void pup(Puper &puper, TransformComponent &comp, const ObjInfo &info) {
		pup(puper, comp.position, util::ObjInfo{ "position" } + info);
		pup(puper, comp.scale, util::ObjInfo{ "scale" } + info);
		pup(puper, comp.rotationAxis, util::ObjInfo{ "rotation_axis" } + info);
		pup(puper, comp.rotationAngle, util::ObjInfo{ "rotation_angle" } + info);
	}

	void pup(Puper &puper, AABB2dComponent &comp, const ObjInfo &info) {
		pup(puper, comp.halfExtent, util::ObjInfo{ "half_extent" } + info);
		pup(puper, comp.offset, util::ObjInfo{ "offset" } + info);
	}

	void pup(Puper &puper, SpriteComponent &comp, const ObjInfo &info) {
		pup(puper, comp.sprite, util::ObjInfo{ "sprite" } + info);
		pup(puper, comp.animFrameX, util::ObjInfo{ "animframe_x" } + info);
		pup(puper, comp.animFrameY, util::ObjInfo{ "animframe_y" } + info);
	}

	void pup(Puper &puper, PhysicsBody2dComponent &comp, const ObjInfo &info) {
		pup(puper, comp.velocity, util::ObjInfo{ "velocity" } + info);
		pup(puper, comp.force, util::ObjInfo{ "force" } + info);
		pup(puper, comp.mass, util::ObjInfo{ "mass" } + info);
		pup(puper, comp.invMass, util::ObjInfo{ "inv_mass" } + info);
		pup(puper, comp.restitution, util::ObjInfo{ "restitution" } + info);
		pup(puper, comp.staticFriction, util::ObjInfo{ "static_friction" } + info);
		pup(puper, comp.dynamicFriction, util::ObjInfo{ "dynamic_friction" } + info);
	}

}