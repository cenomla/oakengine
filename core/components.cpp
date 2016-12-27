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
		pup(puper, comp.position, ObjInfo{ "position" } + info);
		pup(puper, comp.scale, ObjInfo{ "scale" } + info);
		pup(puper, comp.rotationAxis, ObjInfo{ "rotation_axis" } + info);
		pup(puper, comp.rotationAngle, ObjInfo{ "rotation_angle" } + info);
	}

	void pup(Puper &puper, TextComponent &comp, const ObjInfo &info) {
		pup(puper, comp.font, ObjInfo{ "font" } + info);
		pup(puper, comp.text, ObjInfo{ "text" } + info);
	}

	void pup(Puper &puper, AABB2dComponent &comp, const ObjInfo &info) {
		pup(puper, comp.halfExtent, ObjInfo{ "half_extent" } + info);
		pup(puper, comp.offset, ObjInfo{ "offset" } + info);
	}

	void pup(Puper &puper, SpriteComponent &comp, const ObjInfo &info) {
		pup(puper, comp.sprite, ObjInfo{ "sprite" } + info);
		pup(puper, comp.animFrameX, ObjInfo{ "animframe_x" } + info);
		pup(puper, comp.animFrameY, ObjInfo{ "animframe_y" } + info);
	}

	void pup(Puper &puper, PhysicsBody2dComponent &comp, const ObjInfo &info) {
		pup(puper, comp.velocity, ObjInfo{ "velocity" } + info);
		pup(puper, comp.force, ObjInfo{ "force" } + info);
		pup(puper, comp.mass, ObjInfo{ "mass" } + info);
		pup(puper, comp.invMass, ObjInfo{ "inv_mass" } + info);
		pup(puper, comp.restitution, ObjInfo{ "restitution" } + info);
		pup(puper, comp.staticFriction, ObjInfo{ "static_friction" } + info);
		pup(puper, comp.dynamicFriction, ObjInfo{ "dynamic_friction" } + info);
	}

}