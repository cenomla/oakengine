local player = {
	max_speed = 400.0,
	on_update = function(self, dt)
		local tc = self:get_transform()

		if oak.input.keys[87] ~= 0 then tc.position.y = tc.position.y - self.max_speed * dt end
		if oak.input.keys[65] ~= 0 then tc.position.x = tc.position.x - self.max_speed * dt end
		if oak.input.keys[83] ~= 0 then tc.position.y = tc.position.y + self.max_speed * dt end
		if oak.input.keys[68] ~= 0 then tc.position.x = tc.position.x + self.max_speed * dt end

		self:set_transform(tc)

		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.view_set_view(0, t)

	end,
	on_entity_collide = function(self, evt)
		local tc = self:get_transform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:set_transform(tc)


		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.view_set_view(0, t)
	end,
	on_tile_collide = function(self, evt)
		local tc = self:get_transform()

		tc.position.x = tc.position.x - evt.normal.x * evt.depth
		tc.position.y = tc.position.y - evt.normal.y * evt.depth
	
		self:set_transform(tc)


		local t = { x = tc.position.x - 640, y = tc.position.y - 360 }
		oak.view_set_view(0, t)
	end
}

return player