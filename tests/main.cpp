#include <iostream>
#include <worker.h>
#include <resource.h>

int main(int argc, char **argv) {

	std::cout << sizeof(oak::Worker) << std::endl;
	std::cout << sizeof(oak::Resource<int>) << std::endl; 

	return 0;
}