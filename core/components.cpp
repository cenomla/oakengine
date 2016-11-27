#include "components.h"

namespace oak::util {

	void pup(Pupper &pupper, PrefabComponent &comp, ObjInfo &info) {
		pup(pupper, comp.id, info);	
	}

	void pup(Pupper &pupper, TransformComponent &comp, ObjInfo &info) {
		pup(pupper, comp.position.x, info);
		pup(pupper, comp.position.y, info);
		pup(pupper, comp.position.z, info);
		pup(pupper, comp.scale, info);
		pup(pupper, comp.rotationAxis.x, info);
		pup(pupper, comp.rotationAxis.x, info);
		pup(pupper, comp.rotationAxis.x, info);
		pup(pupper, comp.rotationAngle, info);
	}

	void pup(Pupper &pupper, AABB2dComponent &comp, ObjInfo &info) {
		pup(pupper, comp.halfExtent.x, info);
		pup(pupper, comp.halfExtent.y, info);
		pup(pupper, comp.offset.x, info);
		pup(pupper, comp.offset.y, info);
	}

	void pup(Pupper &pupper, SpriteComponent &comp, ObjInfo &info) {
		pup(pupper, comp.sprite, info);
		pup(pupper, comp.animFrameX, info);
		pup(pupper, comp.animFrameY, info);
	}

	void pup(Pupper &pupper, PhysicsBody2dComponent &comp, ObjInfo &info) {
		pup(pupper, comp.velocity.x, info);
		pup(pupper, comp.velocity.y, info);
		pup(pupper, comp.force.x, info);
		pup(pupper, comp.force.y, info);
		pup(pupper, comp.mass, info);
		pup(pupper, comp.invMass, info);
		pup(pupper, comp.restitution, info);
		pup(pupper, comp.staticFriction, info);
		pup(pupper, comp.dynamicFriction, info);
	}

}