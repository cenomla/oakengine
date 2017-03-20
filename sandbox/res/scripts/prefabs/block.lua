local prefab = {
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
}

local block = {

}

return { prefab, block }