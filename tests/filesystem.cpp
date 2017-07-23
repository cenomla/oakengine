#include <cstdio>
#include <file_manager.h>
#include <log.h>

struct stdoutstream : public oak::log::Stream {
	void write(const void *source, size_t size) override {
		fwrite(source, 1, size, stdout);
	}
};

int main(int argc, char **argv) {
	//setup log
	stdoutstream sos;
	oak::log::cout.addStream(&sos);
	oak::log::cwarn.addStream(&sos);
	oak::log::cerr.addStream(&sos);
	

	oak::FileManager fm;

	fm.mount("{$installDir}", "/");
	fm.mount("{$cwd}", "/config");
	fm.mount("{$cwd}/tests/res/options", "/config");
	fm.mount("{$cwd}/tests/res/data/options", "/config");
	fm.mount("{$cwd}/tests/res/options/subdir/settings/how_to_fly", "/config/how_to_fly");


	printf("resolved path: %s -> %s\n", "/config/engine", fm.resolvePath("/config/engine").c_str());
	printf("resolved path: %s -> %s\n", "/config/test", fm.resolvePath("/config/test").c_str());
	printf("resolved path: %s -> %s\n", "/config/how_to_fly/flying101", fm.resolvePath("/config/how_to_fly/flying101").c_str());

	oak::Stream stream = fm.openFile("/config/engine");
	oak::string str = stream.read<oak::string>();
	fm.closeFile(stream);


	printf("%s\n", str.c_str());

	fm.closeFile(stream);

	return 0;
}