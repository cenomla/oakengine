package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)

oak.input = oak.es:create_entity(255, require("input"))

oak.load_shader({
	name = "shd_pass",
	path = "core/graphics/shaders/pass2d"
})

oak.load_shader({
	name = "shd_font",
	path = "core/graphics/shaders/font"
})

oak.load_texture({
	name ="tex_font",
	path = "res/fonts/dejavu_sans/atlas",
	flags = 1
})

oak.load_texture({
	name = "tex_light",
	path = "res/textures/light",
	flags = 0
})

oak.load_atlas({
	name = "atlas_entity",
	textures = {
		"res/textures/character", "res/textures/block", size = 2
	},
	extent = {
		x = 512,
		y = 512
	},
	flags = 0
})

oak.load_atlas({
	name = "atlas_tiles",
	textures = {
		"res/textures/tiles/geometric_wall_1", size = 1
	},
	extent = {
		x = 8000, 
		y = 6400
	},
	flags = 0
})

oak.load_atlas({
	name = "atlas_gui",
	textures = {
		"res/textures/gui/button",
		"res/textures/gui/arrows",
		"res/textures/gui/text_box",
		"res/textures/gui/cursor",
		"res/textures/gui/tile_editor/background",
		"res/textures/gui/tile_editor/icons",
		"res/textures/gui/tile_editor/selector",
		"res/textures/gui/tool_bar/background",
		"res/textures/gui/tool_bar/icons",
		"res/textures/gui/console/background",
		size = 10
	},
	extent = {
		x = 1024,
		y = 1024
	},
	flags = 0
})

oak.load_material({
	name = "mat_entity",
	shader = "shd_pass",
	texture = "atlas_entity"
})

oak.load_material({
	name = "mat_gui",
	shader = "shd_pass",
	texture = "atlas_gui"
})

oak.load_material({
	name = "mat_font",
	shader = "shd_font",
	texture = "tex_font"
})

oak.load_material({
	name = "mat_light",
	shader = "shd_pass",
	texture = "tex_light"
})

oak.set_uniform("shd_font", "text_width", 0.4)
oak.set_uniform("shd_font", "text_edge", 0.4)
oak.set_uniform("shd_font", "text_color", { 1.0, 1.0, 1.0 })
oak.set_uniform("shd_font", "border_width", 0.6)
oak.set_uniform("shd_font", "border_edge", 0.4)
oak.set_uniform("shd_font", "border_color", { 0.0, 0.0, 0.0 })

oak.load_material({
	name = "mat_tiles",
	shader = "shd_pass",
	texture = "atlas_tiles"
})

oak.load_font({
	name = "fnt_dejavu",
	material = "mat_font",
	path = "res/fonts/dejavu_sans"
})

oak.load_sprite({
	name = "spr_player",
	material = "mat_entity",
	center = { x = 8.0, y = 8.0 },
	extent = { x = 16.0, y = 16.0 },
	atlas = "atlas_entity",
	path = "res/textures/character"
})

oak.load_sprite({
	name = "spr_block",
	material = "mat_entity",
	center = { x = 64.0, y = 16.0 },
	extent = { x = 128.0, y = 32.0 },
	atlas = "atlas_entity",
	path = "res/textures/block"
})

oak.load_sprite({
	name = "spr_gui_button",
	material = "mat_gui",
	center = { x = 0.0, y = 0.0 },
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/button",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_tile_editor",
	material = "mat_gui",
	center = { x = 0.0, y = 0.0 },
	extent = { x = 256.0, y = 512.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/tile_editor/background"
})

oak.load_sprite({
	name = "spr_tile_editor_icons",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/tile_editor/icons",
	animframes_x = 5,
	animframes_y = 4
})

oak.load_sprite({
	name = "spr_gui_arrows",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/arrows",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_gui_text_box",
	material = "mat_gui",
	extent = { x = 224.0, y = 24.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/text_box",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_gui_cursor",
	material = "mat_gui",
	extent = { x = 2.0, y = 18.0 },
	center = { x = -1.0, y = -2.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/cursor"
})

oak.load_sprite({
	name = "spr_tile_editor_tilemap",
	material = "mat_tiles",
	extent = { x = 8000.0, y = 6400.0 },
	draw_position = { x = 0.0, y = 0.0 },
	draw_extent = { x = 1.0, y = 1.0 }
})

oak.load_sprite({
	name = "spr_tile_editor_selector",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	center = { x = 8.0, y = 8.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/tile_editor/selector"
})

oak.load_sprite({
	name = "spr_tool_bar",
	material = "mat_gui",
	extent = { x = 176.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/tool_bar/background"
})

oak.load_sprite({
	name = "spr_tool_bar_icons",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/tool_bar/icons",
	animframes_x = 4
})

oak.load_sprite({
	name = "spr_console",
	material = "mat_gui",
	extent = { x = 512.0, y = 256.0 },
	atlas = "atlas_gui",
	path = "res/textures/gui/console/background"
})

oak.es:create_prefab("player", {
	transform = {
		position = {
			x = 216.0,
			y = 128.0
		},
		scale = 1.0
	},
	sprite = {
		shared = true,
		sprite = hash("spr_player")
	},
	light = {

	},
	aabb2d = {
		shared = true,
		half_extent = {
			x = 8.0, 
			y = 8.0	
		}
	},
	physics_body_2d = {
		mass = 0.025 * 0.2,
		inv_mass = 0.0,
		restitution = 0.4,
		static_friction = 0.5,
		dynamic_friction = 0.4	
	},
	text = {
		font = hash("fnt_dejavu"),
		text = "This be the Player!",
		offset = { x = -96.0, y = 8.0 },
		size = 12.0
	}
})

oak.es:create_prefab("block", {
	transform = {
		position = {
			x = 256.0,
			y = 512.0
		},
		scale = 2.0
	},
	sprite = {
		shared = true,
		sprite = hash("spr_block")
	},
	occluder = {

	},
	aabb2d = {
		shared = true,
		half_extent = { x = 128.0, y = 32.0 }
	}, 
	physics_body_2d = {
		mass = 0.0, inv_mass = 0.0,
		restitution = 0.4,
		static_friction = 0.5,
		dynamic_friction = 0.4
	}
})

oak.es:create_prefab("gui_button", {
	transform = {
		position = { z = 1.0 }
	},
	sprite = {
		sprite = hash("spr_gui_button")
	}
})

oak.es:create_prefab("gui_arrow", {
	transform = {
		position = { z = 1.0 }
	},
	sprite = {
		sprite = hash("spr_gui_arrows")
	}
})

oak.es:create_prefab("gui_text_box", {
	transform = {
		position = { z = 1.0 }
	},
	sprite = {
		sprite = hash("spr_gui_text_box")
	},
	aabb2d = {
		half_extent = { x = 32.0, y = 8.0 },
		offset = { x = 32.0, y = 8.0 }
	},
	text = {
		font = hash("fnt_dejavu"),
		offset = { x = 2.0, y = 2.0 },
		size = 12.0
	}
})

oak.es:create_prefab("tile_editor", {
	transform = {
		position = { y = 32.0 }
	},
	sprite = {
		sprite = hash("spr_tile_editor")
	}
})

oak.es:create_prefab("entity_editor", {
	transform = {
		position = { y = 32.0 }
	}
})

oak.es:create_prefab("console", {
	transform = {
		position = { y = 32.0 }
	},
	sprite = {
		sprite = hash("spr_console")
	}
})

oak.es:create_prefab("debug", {
	transform = {
		position = { y = 32.0 }
	}
})

oak.es:create_prefab("tool_bar", {
	transform = {
	},
	sprite = {
		sprite = hash("spr_tool_bar")
	}
})

oak.es:create_entity(0, "player", require("player"))
oak.es:create_entity(0, "block", {})

local e = oak.es:create_entity(0, "block", {})
e:set_transform({ position = { x = 512, y = 496 } })

oak.es:create_entity(2, "tool_bar", require("gui/tool_bar"))