local prefab = {
	transform = {
		position = { x = 32.0, y = 32.0 }
	},
	aabb2d = {
		half_extent = { x = 256.0, y = 144.0 },
		offset = { x = 256.0, y = 144.0 }
	},
	sprite = {
		sprite = hash("spr_window")
	}
}

local window = {
	on_create = function(self)
		self.children = {}
	end,
	update_children = function(self)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()
		local x = 16
		local y = 16
		local lh = 0
		for k, v in pairs(self.children) do
			local ctc = v:get_transform()
			local caabb = v:get_aabb2d()

			if caabb.half_extent.y * 2 > lh then
				lh = caabb.half_extent.y * 2
			end

			if x + caabb.half_extent.x * 2 > aabb.half_extent.x * 2 - 16 then
				x = 16
				y = y + lh + 16
				lh = 0
			end

			ctc.position.x = tc.position.x + x
			ctc.position.y = tc.position.y + y
			v:set_transform(ctc)

			x = x + caabb.half_extent.x * 2 + 16

		end
	end,
	on_mouse_move = function(self, evt)
		if oak.input.buttons[0] ~= 0 then
			local tc = self:get_transform()
			local aabb = self:get_aabb2d()
			local vmx = oak.vs.transform_point(oak.vs.get_id(self:layer()), evt)

			if point_intersects(tc, aabb, vmx) then
				tc.position = { x = tc.position.x + oak.input.dmx, y = tc.position.y + oak.input.dmy }
				self:set_transform(tc)
				self:update_children()
			end
		end
	end,
	on_activate = function(self)
		for k, v in pairs(self.children) do
			v:activate()
		end
	end,
	on_deactivate = function(self)
		for k, v in pairs(self.children) do
			v:deactivate()
		end
	end
}

return { prefab, window }