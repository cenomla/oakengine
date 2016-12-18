local button = {
	mx = 0,
	my = 0,
	air = {
		dx = 0.0,
		dy = 0.0,
		dw = 0.0,
		dh = 0.0,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 0.0,
		height = 0.0,
		animframe_x = 0,
		animframe_y = 0,
		flags = 0
	},
	grass = {
		dx = 0.1875,
		dy = 0.0,
		dw = 0.0625,
		dh = 0.0625,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 16.0,
		height = 16.0,
		animframe_x = 0,
		animframe_y = 0,
		flags = 3
	},
	create = function(self)
		self.buttons = {}
		for i = 0, 32 do
			self.buttons[i] = 0
		end
	end,
	update = function(self)
		local chunk = oak.ts:getChunk(self.mx, self.my)
		if self.buttons[0] ~= 0 then 
			chunk:setTile(self.mx//16 % 16, self.my//16 % 16, self.air)
		elseif self.buttons[1] ~= 0 then
			chunk:setTile(self.mx//16 % 16, self.my//16 % 16, self.grass)
		end

	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		self.buttons[button] = action
		return true
	end

}

return button