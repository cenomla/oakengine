bench = executable(
	'bench', 
	'benchmark.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

buffer = executable(
	'buffer', 
	'buffer.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

equeue = executable(
	'equeue', 
	'equeue.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

filesystem = executable(
	'filesystem', 
	'filesystem.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

resource_handler = executable(
	'resource_handler', 
	'resource_handler.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

math = executable(
	'math', 
	'math.cpp', 
	include_directories : lib_includes, 
	link_with : oak, 
	dependencies : deps, 
	cpp_args : '-std=c++17')

test('bench', bench)
test('buffer', buffer)
test('equeue', equeue)
test('filesystem', filesystem)
test('resource_handler', resource_handler)
test('math', math)
