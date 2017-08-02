#build glad

glad_source = ['glad/glad.c']
glad_include = include_directories('glad/include')

glad = static_library('glad', glad_source, include_directories : glad_include)

#build glfw
glfw_sources_in = ['context.c', 'init.c', 'input.c', 'linux_joystick.c', 'monitor.c', 'posix_time.c', 'posix_tls.c', 'vulkan.c', 
	'window.c', 'glx_context.c', 'egl_context.c', 'x11_init.c', 'x11_monitor.c', 'x11_window.c', 'xkb_unicode.c']
glfw_sources = []
glfw_include = include_directories('glfw-3.2.1/include/')

foreach s : glfw_sources_in
	glfw_sources += join_paths('glfw-3.2.1/src', s)
endforeach

glfw = static_library('glfw', glfw_sources, c_args : '-D_GLFW_X11')

#build libsoundio
sio_sources_in = ['channel_layout.c', 'dummy.c', 'os.c', 'pulseaudio.c', 'ring_buffer.c', 'soundio.c', 'util.c']
sio_sources = []
sio_include = include_directories('soundio/include/')

foreach s : sio_sources_in
	sio_sources += join_paths('soundio/src', s)
endforeach

sio = static_library('soundio', sio_sources, include_directories : sio_include)

#build lua
lua_sources_in = ['lapi.c', 'lauxlib.c', 'lbaselib.c', 'lbitlib.c', 'lcode.c', 'lcorolib.c', 'lctype.c', 'ldblib.c', 'ldebug.c', 'ldo.c', 'ldump.c', 
	'lfunc.c', 'lgc.c', 'linit.c', 'liolib.c', 'llex.c', 'lmathlib.c', 'lmem.c', 'loadlib.c', 'lobject.c', 'lopcodes.c', 'loslib.c', 'lparser.c', 
	'lstate.c', 'lstring.c', 'lstrlib.c', 'ltable.c', 'ltablib.c', 'ltm.c', 'lua.c', 'luac.c', 'lundump.c', 'lutf8lib.c', 'lvm.c', 'lzio.c']
lua_sources = []
lua_include = include_directories('lua-5.3.3/')

foreach s : lua_sources_in
	lua_sources += join_paths('lua-5.3.3/lua', s)
endforeach

lua = static_library('lua', lua_sources)

#add includes from header only libraries

glm_include = include_directories('glm')

stb_include = include_directories('stb')

lib_includes = [glad_include, glfw_include, sio_include, lua_include, glm_include, stb_include]

