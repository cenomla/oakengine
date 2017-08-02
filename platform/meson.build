platform_sources = [
	'components.cpp',
	'console.cpp',
	'main.cpp',
	'render_system.cpp']


platform_deps = [
	dependency('assimp'), 
	dependency('vulkan'),
	dependency('GL'),
	dependency('x11'),
	dependency('xrandr'),
	dependency('xinerama'),
	dependency('xcursor'),
	dependency('xxf86vm'),
	dependency('xi')]

compiler = meson.get_compiler('cpp')

platform_deps += compiler.find_library('m', required : false)
platform_deps += compiler.find_library('pthread', required : false)
platform_deps += compiler.find_library('dl', required : false)

platform = executable('platform', platform_sources, include_directories : lib_includes, link_with : oak, dependencies : platform_deps, cpp_args : '-std=c++17', link_args : '-lstdc++fs')
