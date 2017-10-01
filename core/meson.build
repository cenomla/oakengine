

oak_sources = [
	'allocators.cpp',
	'audio_manager.cpp',
	'collision.cpp',
	'component_storage.cpp',
	'core_components.cpp',
	'debugger.cpp',
	'debug_vars.cpp',
	'entity_cache.cpp',
	'entity_id.cpp',
	'event_manager.cpp',
	'event_queue.cpp',
	'file_manager.cpp',
	'input_events.cpp',
	'input_manager.cpp',
	'log.cpp',
	'luah.cpp',
	'lua_puper.cpp',
	'lua_system.cpp',
	'oak_alloc.cpp',
	'prefab.cpp',
	'resource_manager.cpp',
	'scene.cpp',
	'scene_events.cpp',
	'system.cpp',
	'system_manager.cpp',
	'update_events.cpp',

	'graphics/buffer.cpp',
	'graphics/buffer_storage.cpp',
	'graphics/camera.cpp',
	'graphics/font.cpp',
	'graphics/framebuffer.cpp',
	'graphics/gl_api.cpp',
	'graphics/gl_buffer.cpp',
	'graphics/gl_framebuffer.cpp',
	'graphics/gl_shader.cpp',
	'graphics/gl_texture.cpp',
	'graphics/gl_vertex_array.cpp',
	'graphics/mesh.cpp',
	'graphics/particle_system.cpp',
	'graphics/shader.cpp',
	'graphics/sprite_batcher.cpp',
	'graphics/static_batcher.cpp',
	'graphics/texture.cpp',
	'graphics/material.cpp',
	'graphics/sprite.cpp',
	'graphics/vk_api.cpp',

	'util/byte_buffer.cpp',
	'util/file_buffer.cpp',
	'util/puper.cpp',
	'util/stream.cpp',
	'util/stream_puper.cpp',
	
	'math/vec.cpp',
	'math/mat.cpp',
	'math/quat.cpp',
	'math/util.cpp']

oak_include = include_directories('.')



oak = static_library(
	'oak', 
	oak_sources, 
	include_directories : lib_includes, 
	cpp_args : '-std=c++17', 
	objects : [
		glad.extract_all_objects(), 
		glfw.extract_all_objects(), 
		sio.extract_all_objects(),
		lua.extract_all_objects(), 
		stb.extract_all_objects()
	])

lib_includes += oak_include
