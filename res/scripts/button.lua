local button = {
	mx = 0,
	my = 0,
	create = function(self)
		self:setPosition(0, 0, 0)
		self:setText("txt_play")
	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		if button == 0 and action ~= 0 then
			local x, y = self:getPosition()
			if self.mx > x and self.mx < x + 96 and self.my > y and self.my < y + 24 then
				print("button pressed!")
				return true
			end
		end
	end

}

return button