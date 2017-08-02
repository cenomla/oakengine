#include <cstdio>
#include <util/file_buffer.h>
#include <file_manager.h>
#include <log.h>

int main(int argc, char **argv) {
	//setup log
	oak::FileBuffer fb{ stdout };
	oak::Stream ls{ &fb };
	oak::log::cout.addStream(&ls);
	oak::log::cwarn.addStream(&ls);
	oak::log::cerr.addStream(&ls);
	

	oak::FileManager fm;

	fm.mount("{$installDir}", "/");
	fm.mount("{$installDir}", "/config");
	fm.mount("{$installDir}/tests/res/options", "/config");
	fm.mount("{$installDir}/tests/res/data/options", "/config");
	fm.mount("{$installDir}/tests/res/options/subdir/settings/how_to_fly", "/config/how_to_fly");


	printf("resolved path: %s -> %s\n", "/config/engine", fm.resolvePath("/config/engine").c_str());
	printf("resolved path: %s -> %s\n", "/config/test", fm.resolvePath("/config/test").c_str());
	printf("resolved path: %s -> %s\n", "/config/how_to_fly/flying101", fm.resolvePath("/config/how_to_fly/flying101").c_str());

	oak::Stream stream = fm.openFile("/config/engine");
	oak::string str = stream.read<oak::string>();
	fm.closeFile(stream);


	printf("%s\n", str.c_str());

	return 0;
}