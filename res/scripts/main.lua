package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)

oak.input = oak.es:create_entity(255, require("input"))

oak.es.manager:create_prefab("player", {
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
	aabb_2d = {
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
		text = "This be the Player!"
	}
})

oak.es.manager:create_prefab("block", {
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
	aabb_2d = {
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

oak.es.manager:create_prefab("button", {
	transform = {
		position = { z = 1.0 }
	},
	sprite = {
		sprite = hash("spr_button")
	}
})

oak.es.manager:create_prefab("tile_editor", {
	transform = {
		position = { y = 48.0 }
	},
	sprite = {
		sprite = hash("spr_tile_editor")
	}
})

oak.es:create_entity(0, "player", require("player"))
oak.es:create_entity(0, "block", {})

local e = oak.es:create_entity(0, "block", {})
e:setTransform({ position = { x = 512, y = 496 } })

local te = oak.es:create_entity(1, "tile_editor", require("gui/tile_editor"))

e = oak.es:create_entity(1, "button", require("gui/button"))
e.callback = function(button) te:on_press(button) end

e = oak.es:create_entity(1, "button", require("gui/button"))
e:setTransform({ position = { x = 48 }})
