#include "components.h"

#include <pup.h>

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.transform, oak::ObjInfo::make<glm::mat3>(&info, "transform"));
}

void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.velocity, oak::ObjInfo::make<glm::vec2>(&info, "velocity"));
	pup(puper, data.acceleration, oak::ObjInfo::make<glm::vec2>(&info, "acceleration"));
}

void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.mesh, oak::ObjInfo::make<oak::Resource<oak::Mesh2d>>(&info, "mesh"));
}

void pup(oak::Puper& puper, RigidBodyComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.restitution, oak::ObjInfo::make<float>(&info, "restitution"));
	pup(puper, data.mass, oak::ObjInfo::make<float>(&info, "mass"));
	pup(puper, data.invMass, oak::ObjInfo::make<float>(&info, "invMass"));
}

void pup(oak::Puper& puper, SpriteComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.sprite, oak::ObjInfo::make<oak::Resource<oak::graphics::Sprite>>(&info, "sprite"));
	pup(puper, data.material, oak::ObjInfo::make<oak::Resource<oak::graphics::Material>>(&info, "material"));
	pup(puper, data.layer, oak::ObjInfo::make<uint32_t>(&info, "layer"));
}

void pup(oak::Puper& puper, TextComponent& data, const oak::ObjInfo& info) {
	pup(puper, data.text, oak::ObjInfo::make<oak::string>(&info, "text"));
	pup(puper, data.font, oak::ObjInfo::make<oak::Resource<oak::graphics::Font>>(&info, "font"));
	pup(puper, data.material, oak::ObjInfo::make<oak::Resource<oak::graphics::Material>>(&info, "material"));
	pup(puper, data.layer, oak::ObjInfo::make<uint32_t>(&info, "layer"));
}

