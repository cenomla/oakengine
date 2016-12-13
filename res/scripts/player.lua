
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
	end,
	key_press = function(self, key, scancode, action, mods)
		self.keys[key] = action
		local chunk = oak.ts:getChunk(0, 0)
		local tile = chunk:getTile(0, 0)
		print(tile.dw)
		print(tile.dh)
		print(tile.flags)
	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		if button == 0 and action ~= 0 then
			local e = oak.es:createEntity(0, "player")
			e:setTransform({position = { x = self.mx, y = self.my }})

			return true
		end
	end
}

return player