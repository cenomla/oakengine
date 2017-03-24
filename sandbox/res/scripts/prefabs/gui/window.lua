local prefab = {
	transform = {
		position = { x = 32.0, y = 32.0 }
	},
	aabb2d = {
		half_extent = { x = 256.0, y = 144.0 },
		offset = { x = 256.0, y = 144.0 }
	},
	sprite = {
		sprite = hash("spr_window"),
		scale = { x = 512.0, y = 288.0 },
		layer = 2
	}
}

local window = {
	active = {},
	on_create = function(self)
		self.children = {}
		self.title = oak.es:create_entity(self:depth() + 0.1, "title_bar")
		self.close = make_button(self:depth() + 0.2, 520.0, 8.0, "spr_button", 32.0, 32.0, nil, 
			function(b, button, action) if button == 0 and action == 0 then self:deactivate() end end)
		self.padding = 8
	end,
	update_children = function(self)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()
		self.title:set_transform({ position = { x = tc.position.x, y = tc.position.y } })
		self.close:set_transform({ position = { x = tc.position.x + 488.0, y = tc.position.y - 8.0 } })
		self.title:set_depth(self:depth() + 0.1)
		self.close:set_depth(self:depth() + 0.2)
		local x = self.padding
		local y = self.padding + 16.0
		local lh = 0
		for k, v in pairs(self.children) do
			local ctc = v:get_transform()
			local caabb = v:get_aabb2d()

			if caabb.half_extent.y * 2 > lh then
				lh = caabb.half_extent.y * 2
			end

			if x + caabb.half_extent.x * 2 > aabb.half_extent.x * 2 - self.padding then
				x = self.padding
				y = y + lh + self.padding
				lh = 0
			end

			ctc.position.x = tc.position.x + x
			ctc.position.y = tc.position.y + y
			v:set_transform(ctc)
			v:set_depth(self:depth() + 0.1)

			x = x + caabb.half_extent.x * 2 + self.padding

		end
	end,
	on_mouse_move = function(self, evt)
		if self.active ~= self then return end
		if oak.input.buttons[0] ~= 0 then
			local tc = self.title:get_transform()
			local aabb = self.title:get_aabb2d()
			local vmx = oak.vs.transform_point(oak.vs.get_id(2), { x = evt.x - oak.input.dmx, y = evt.y - oak.input.dmy })

			if point_intersects(tc, aabb, vmx) then
				tc = self:get_transform()
				tc.position = { x = tc.position.x + oak.input.dmx, y = tc.position.y + oak.input.dmy }
				self:set_transform(tc)
				self:update_children()
			end
		end
	end,
	on_button_press = function(self, evt)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()
		local vmx = oak.vs.transform_point(oak.vs.get_id(2), { x = oak.input.mx, y = oak.input.my })
		if point_intersects(tc, aabb, vmx) then
			self.active = self
			return true --capture event
		end
	end,
	on_activate = function(self)
		for k, v in pairs(self.children) do
			v:activate()
		end
		self.title:activate()
		self.close:activate()
	end,
	on_deactivate = function(self)
		for k, v in pairs(self.children) do
			v:deactivate()
		end
		self.title:deactivate()
		self.close:deactivate()
	end
}

return { prefab, window }