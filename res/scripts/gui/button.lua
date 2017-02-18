require("util/collision")

local button = {
	active = 0,
	set_active = function(self, active)
		self:set_sprite({ animframe_y = active })
		self.active = active
		if self.callback ~= nil then 
			self:callback(active)
		end
	end,
	toggle_active = function(self, active)
		if self.active == 0 then
			self:set_active(1)
		else 
			self:set_active(0)
		end
	end,
	on_button_press = function(self, evt)
		local tc = self:get_transform()
		local aabb = { half_extent = { x = 16.0, y = 16.0 }, offset = { x = 16.0, y = 16.0 } }
		if self:has_aabb2d() then
			aabb = self:get_aabb2d()
		end

		local vmx = oak.vs.transform_point(oak.vs.get_id(self:layer()), { x = oak.input.mx, y = oak.input.my })

		local sp = self:get_sprite()
		if evt.action == 0 then self:set_sprite({ animframe_x = 0 }) end
		if point_intersects(tc, aabb, { x = vmx.x, y = vmx.y }) then
			if evt.button == 0 then
				if evt.action == 0 then
					if sp.animframe_x == 1 then self:toggle_active() end
				else
					self:set_sprite({ animframe_x = 1 })
				end
				return true
			end
		end
		
	end

}

return button