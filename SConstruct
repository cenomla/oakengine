import os
import fnmatch

#helper functions
def get_sources_recursive(file_list, source, build, regex, rel='/'):
	for f in os.listdir(source + rel):
		if fnmatch.fnmatch(f, regex):
			file_list += [build + rel + f]
		if os.path.isdir(source + rel + f):
			get_sources_recursive(file_list, source, build, regex, rel + f + '/')


#config
include_path = ['src', 'lib/include']
lib_path = 'lib'
src_path = 'src'
intermediate_path = 'bin/oak.d'
target_path = 'bin/oak'
flags = ['-Wall', '-g', '-fno-exceptions', '-fno-rtti', '-D _DEBUG']

#x11 libs
#'Xinerama', 'Xxf86vm', 'X11', 'Xcursor', 'Xrandr'
#wayland libs
#'wayland-cursor', 'wayland-client', 'wayland-egl', 'xkbcommon'
#lists
sources = []
#libs = ['lua', 'GL', 'glad', 'glfw3', 'wayland-cursor', 'wayland-client', 'wayland-egl', 'xkbcommon', 'pthread', 'dl', 'm']
libs = ['pthread', 'dl', 'm']

get_sources_recursive(sources, src_path, intermediate_path, '*.cpp')

#build
env = Environment(CXXFLAGS = flags, CPPPATH = include_path, LIBPATH = lib_path, LIBS = libs)

env.VariantDir(intermediate_path, src_path, duplicate=0)
env.Program(target = target_path, source = sources)