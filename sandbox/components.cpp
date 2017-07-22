#include "components.h"


void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, Transform2dComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, CameraComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, SpriteComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, TextComponent& data, const oak::ObjInfo& info) {}