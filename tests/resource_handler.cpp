#include <iostream>
#include <resource_manager.h>
#include <container.h>
#include <oak_alloc.h>

struct Res {
	oak::vector<int> list;
	float x, y;
};

int main(int argc, char **argv) {

	oak::PoolAllocator alloc{ &oak::oalloc_freelist, oak::operator""_mb(48), sizeof(Res) };

	oak::ResourceManager resManager;

	auto& res = resManager.get<Res>().add("mamoth", oak::vector<int>{ 95, 64, 12 }, 55.5f, 90.0f);

	auto& r = resManager.require<Res>("mamoth");

	r.x = -1.0f;
	res.y = -20.0f;

	printf("r: %p, res: %p\n", &r, &res);
	printf("x: %f\n", res.x);


	return 0;
}