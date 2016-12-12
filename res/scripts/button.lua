local button = {
	mx = 0,
	my = 0,
	create = function(self)
	end,
	mouse_move = function(self, x, y)
		self.mx = x
		self.my = y
	end,
	button_press = function(self, button, action, mods)
		if button == 0 and action ~= 0 then
			local tc = self:getTransform()
			local x = tc.position.x
			local y = tc.position.y
			if self.mx > x and self.mx < x + 96 and self.my > y and self.my < y + 24 then
				print("button pressed!")
				return true
			end
		end
	end

}

return button