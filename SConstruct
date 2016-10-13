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
include_path = ['core', 'lib/include']
lib_path = ['lib', 'bin']
src_path = 'core'
intermediate_path = 'bin/oak.d'
target_path = 'bin/oak'
flags = ['-std=c++1z', '-Wall', '-g', '-O3', '-fno-exceptions', '-fno-rtti', '-D _DEBUG', '-Wno-pragmas']

#lists
sources = []
wayland_libs = ['glfw3', 'wayland-cursor', 'wayland-client', 'wayland-egl', 'xkbcommon']
xlibs = ['glfw3x', 'X11', 'Xxf86vm', 'Xinerama', 'Xrandr', 'Xcursor']
libs = ['vulkan', 'GL', 'glad', 'lua'] + wayland_libs + ['xkbcommon', 'pthread', 'dl', 'm']

sandbox = []

get_sources_recursive(sources, src_path, intermediate_path, '*.cpp')
get_sources_recursive(sandbox, 'sandbox', 'bin/sandbox.d', '*.cpp')


#build
env = Environment(CXX = 'g++', CXXFLAGS = flags, CPPPATH = include_path, LIBPATH = lib_path)

env.VariantDir(intermediate_path, src_path, duplicate=0)
env.Library(target = target_path, source = sources)

env.VariantDir('bin/sandbox.d', 'sandbox', duplicate=0)
env.Program(target = 'bin/sandbox', LIBS = ['oak'] + libs, source = sandbox)