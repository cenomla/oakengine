local button = {
	active = 0,
	toggle_active = function(self)
		if self.active == 0 then
			self.active = 1
		else 
			self.active = 0
		end
		self:setSprite({ animframe_x = self.active })
	end,
	button_press = function(self, evt)
		local tc = self:getTransform()
		
		if oak.input.mx > tc.position.x and oak.input.mx < tc.position.x + 48 and 
		oak.input.my > tc.position.y and oak.input.my < tc.position.y + 48 then
			if evt.button == 0 and evt.action ~= 0 then
				self:toggle_active()
				if self.callback ~= nil then 
					self:callback()
				end
				return true
			end
		end
		
	end

}

return button