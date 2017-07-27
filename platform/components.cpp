#include "components.h"

#include <pup.h>

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, RigidBodyComponent& data, const oak::ObjInfo& info) {}
