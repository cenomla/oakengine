#include <iostream>
#include <resource.h>
#include <oakengine.h>

struct Res {
	static const oak::TypeInfo typeInfo;

	oak::vector<int> list;
	float x, y;
};

const oak::TypeInfo Res::typeInfo = oak::makeResourceInfo<Res>("res");

void pup(oak::Puper& puper, Res& data, const oak::ObjInfo& info) {}

int main(int argc, char **argv) {

	oak::ResourceManager resManager;

	auto& res = oak::addResource<Res>("mamoth", oak::vector<int>{ 95, 64, 12 }, 55.5f, 90.0f);


	auto& r = oak::requireResource<Res>("mamoth");

	r.x = -1.0f;
	res.y = -20.0f;

	printf("r: %p, res: %p\n", &r, &res);
	printf("x: %f\n", res.x);


	return 0;
}
