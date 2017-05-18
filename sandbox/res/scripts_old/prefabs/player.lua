local prefab = {
	transform = {
		position = {
			x = 216.0,
			y = 128.0
		},
		scale = 1.0
	},
	sprite = {
		shared = true,
		sprite = hash("spr_player"),
		layer = 0
	},
	light = {
		size = 512
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
		shared = true,
		font = hash("fnt_dejavu"),
		text = "Player",
		offset = { x = -24.0, y = -24.0 },
		size = 12.0,
		layer = 0
	}
}

local player = {
	max_speed = 800.0,
	on_update = function(self, dt)
		local tc = self:get_transform()

		if oak.input.keys[87] ~= 0 then tc.position.y = tc.position.y - self.max_speed * dt end
		if oak.input.keys[65] ~= 0 then tc.position.x = tc.position.x - self.max_speed * dt end
		if oak.input.keys[83] ~= 0 then tc.position.y = tc.position.y + self.max_speed * dt end
		if oak.input.keys[68] ~= 0 then tc.position.x = tc.position.x + self.max_speed * dt end

		self:set_transform(tc)

		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.vs.set_view(0, t)

	end,
	on_entity_collide = function(self, evt)
		local tc = self:get_transform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:set_transform(tc)


		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.vs.set_view(0, t)
	end,
	on_tile_collide = function(self, evt)
		local tc = self:get_transform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:set_transform(tc)


		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.vs.set_view(0, t)
	end
}

return { prefab, player }