
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
		local vx, vy = self:getVelocity()

		if self.keys[87] ~= 0 then
			self:addForce(0.0, -25000.0 * self:getMass())
			self.keys[87] = 0
		end
		if self.keys[65] ~= 0 then
			if vx > -self.maxSpeed then
				vx = vx - 40.0
			end
		end
		if self.keys[68] ~= 0 then
			if vx < self.maxSpeed then
				vx = vx + 40.0
			end
		end

		self:setVelocity(vx, vy)
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
			local e = oak.es:createEntity(0, "player")
			e:setPosition(self.mx, self.my)
			return true
		end
	end
}

return player