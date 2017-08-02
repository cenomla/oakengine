project('oak_engine', 'c', 'cpp')

deps = [
	dependency('assimp'), 
	dependency('vulkan'),
	dependency('GL'),
	dependency('x11'),
	dependency('xrandr'),
	dependency('xinerama'),
	dependency('xcursor'),
	dependency('xxf86vm'),
	dependency('xi')]

cxx_compiler = meson.get_compiler('cpp')

deps += cxx_compiler.find_library('stdc++fs', required : true)
deps += cxx_compiler.find_library('m', required : false)
deps += cxx_compiler.find_library('pthread', required : false)
deps += cxx_compiler.find_library('dl', required : false)

subdir('lib')
subdir('core')
subdir('sandbox')
subdir('platform')
subdir('tests')
