local point_intersects = function(transform, aabb, point)
	
	local minx = transform.position.x + aabb.offset.x
	local miny = transform.position.y + aabb.offset.y
	local maxx = minx + aabb.half_extent.x * 2 * transform.scale
	local maxy = miny + aabb.half_extent.y * 2 * transform.scale

	if point.x > minx and point.x < maxx and point.y > miny and point.y < maxy then
		return true
	end
	return false
end

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
		local aabb = { half_extent = { x = 24.0, y = 24.0 }, offset = { x = 0, y = 0 } }

		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
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