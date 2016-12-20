
local player = {
	maxSpeed = 400.0,
	mx = 0,
	my = 0,
	create = function(self)
		self.keys = {}
		for i = 0, 1024 do
			self.keys[i] = 0
		end
	end,
	update = function(self, dt)
		local tc = self:getTransform()

		if self.keys[87] ~=0 then tc.position.y = tc.position.y - self.maxSpeed * dt end
		if self.keys[65] ~= 0 then tc.position.x = tc.position.x - self.maxSpeed * dt end
		if self.keys[83] ~= 0 then tc.position.y = tc.position.y + self.maxSpeed * dt end
		if self.keys[68] ~= 0 then tc.position.x = tc.position.x + self.maxSpeed * dt end

		self:setTransform(tc)
	end,
	key_press = function(self, key, scancode, action, mods)
		self.keys[key] = action
	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		if button == 0 and action ~= 0 then
			local e = oak.es:create_entity(0, "player")
			e:setTransform({position = { x = self.mx, y = self.my }})

			return true
		end
	end,
	entity_collide = function(self, other, normal, depth)
		local tc = self:getTransform()

		tc.position.x = tc.position.x - normal.x * depth
		tc.position.y = tc.position.y - normal.y * depth
	
		self:setTransform(tc)
	end,
	tile_collide = function(self, tile, normal, depth)
		local tc = self:getTransform()

		tc.position.x = tc.position.x - normal.x * depth
		tc.position.y = tc.position.y - normal.y * depth
	
		self:setTransform(tc)
	end
}

return player