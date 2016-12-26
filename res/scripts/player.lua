local player = {
	max_speed = 400.0,
	create = function(self)
	end,
	update = function(self, dt)
		local tc = self:getTransform()

		if oak.input.keys[87] ~= 0 then tc.position.y = tc.position.y - self.max_speed * dt end
		if oak.input.keys[65] ~= 0 then tc.position.x = tc.position.x - self.max_speed * dt end
		if oak.input.keys[83] ~= 0 then tc.position.y = tc.position.y + self.max_speed * dt end
		if oak.input.keys[68] ~= 0 then tc.position.x = tc.position.x + self.max_speed * dt end

		self:setTransform(tc)
	end,
	entity_collide = function(self, evt)
		local tc = self:getTransform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:setTransform(tc)
	end,
	tile_collide = function(self, evt)
		local tc = self:getTransform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:setTransform(tc)
	end
}

return player