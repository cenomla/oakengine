#include <iostream>
#include <util/byte_buffer.h>
#include <oak_alloc.h>

int main(int argc, char **argv) {

	const int32_t i32 = -2000000000;
	const uint64_t u64 = 9000000000000;
	const int16_t i16 = 901;
	const float f32 = 10.001;

	oak::ByteBuffer buffer{ 512 };

	oak::Stream stream{ &buffer };

	stream.write(i32);
	stream.write(u64);
	stream.write(i16);
	stream.write(f32);
	stream.write("hello world");

	buffer.rewind();

	int32_t r0 = stream.read<int32_t>();
	uint64_t r1 = stream.read<uint64_t>();
	int16_t r2 = stream.read<int16_t>();
	float r3 = stream.read<float>();
	oak::string str = stream.read<oak::string>();

	std::cout << "i32: " << r0 << ", u64: " << r1 << ", i16: " << r2 << ", f32: " << r3 << ", str: " << str << std::endl;
	if (i32 != r0 || u64 != r1 || i16 != r2 || f32 != r3) {
		return 1;
	}

	return 0;

}