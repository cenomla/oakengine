sandbox_sources = [
	'components.cpp',
	'deferred_renderer.cpp',
	'main.cpp',
	'render_system.cpp',
	'sprite_renderer.cpp']

sandbox = executable(
	'sandbox', 
	sandbox_sources, 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

