#include <iostream>
#include <util/byte_buffer.h>
#include <oak_alloc.h>

int main(int argc, char **argv) {

	const int32_t i32 = -2000000000;
	const uint64_t u64 = 9000000000000;
	const int16_t i16 = 901;
	const float f32 = 10.001;

	void *mem = oak::oak_allocator.allocate(512);
	oak::util::ByteBuffer buffer{ mem, 512 };
	buffer.write(i32);
	buffer.write(u64);
	buffer.write(i16);
	buffer.write(f32);
	buffer.write("hello world");

	buffer.rewind();

	int32_t r0 = buffer.read<int32_t>();
	uint64_t r1 = buffer.read<uint64_t>();
	int16_t r2 = buffer.read<int16_t>();
	float r3 = buffer.read<float>();
	oak::string str = buffer.read<oak::string>();

	std::cout << "i32: " << r0 << ", u64: " << r1 << ", i16: " << r2 << ", f32: " << r3 << ", str: " << str << std::endl;
	if (i32 != r0 || u64 != r1 || i16 != r2 || f32 != r3) {
		return 1;
	}

	oak::oak_allocator.deallocate(mem, 512);

	return 0;

}