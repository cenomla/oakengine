local button = {
	mx = 0,
	my = 0,
	active = 0,
	create = function(self)
		self.buttons = {}
		for i = 0, 32 do
			self.buttons[i] = 0
		end
	end,
	update = function(self)
		if self.active == 1 then
			local tiles = require "tiles"
			local chunk = oak.ts:getChunk(self.mx, self.my)
			if self.buttons[0] ~= 0 then 
				chunk:setTile(self.mx//16 % 16, self.my//16 % 16, tiles.air)
			elseif self.buttons[1] ~= 0 then
				chunk:setTile(self.mx//16 % 16, self.my//16 % 16, tiles.grass)
			end
		end

	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		self.buttons[button] = action
		local tc = self:getTransform()
		
		if self.mx > tc.position.x and self.mx < tc.position.x + 48 and 
		self.my > tc.position.y and self.my < tc.position.y + 48 and action ~= 0 then
			if self.active == 0 then
				self.active = 1
			else 
				self.active = 0
			end
			self:setSprite({ animframe_x = self.active })
			return true
		elseif button == 0 and action ~= 0 then
			local e = oak.es:create_entity(0, "player")
			e:setTransform({position = { x = self.mx, y = self.my }})

			return true
		end
		
	end

}

return button