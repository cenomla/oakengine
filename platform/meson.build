platform_sources = [
	'components.cpp',
	'console.cpp',
	'main.cpp',
	'render_system.cpp']


platform = executable(
	'platform', 
	platform_sources, 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')
