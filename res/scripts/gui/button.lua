require("util/collision")

local button = {
	active = 0,
	set_active = function(self, active)
		self:setSprite({ animframe_y = active })
		if self.active ~= active then
			self.active = active
		end
	end,
	toggle_active = function(self)
		if self.active == 0 then
			self:set_active(1)
		else 
			self:set_active(0)
		end
		if self.callback ~= nil then 
			self:callback()
		end
	end,
	button_press = function(self, evt)
		local tc = self:getTransform()
		local aabb = { half_extent = { x = 16.0, y = 16.0 }, offset = { x = 16.0, y = 16.0 } }

		local sp = self:getSprite()
		if evt.action == 0 then self:setSprite({ animframe_x = 0 }) end
		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
			if evt.button == 0 then
				if evt.action == 0 then
					if sp.animframe_x == 1 then self:toggle_active() end
				else
					self:setSprite({ animframe_x = 1 })
				end
				return true
			end
		end
		
	end

}

return button