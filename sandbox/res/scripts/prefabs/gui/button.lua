require("util/collision")
local prefab = {
	transform = {
		position = { }
	},
	aabb2d = {
		offset = { x = 16.0, y = 16.0 },
		half_extent = { x = 16.0, y = 16.0 }
	},
	sprite = {
		sprite = hash("spr_gui_button"),
		layer = 2
	}
}

local button = {
	on_create = function(self)
		self.active = false
		self.callback = function(self, button, action) end
		self.hover = false
	end,

	on_mouse_move = function(self, evt)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()
		local vmx = oak.vs.transform_point(oak.vs.get_id(2), evt)
	

		if point_intersects(tc, aabb, vmx) then
			self.hover = true
			if oak.input.buttons[0] ~= 0 or oak.input.buttons[1] ~= 0 then
				self:set_sprite({ animframe_x = 1 })
			else
				self:set_sprite({ animframe_x = 2 })
			end
		else
			self.hover = false
			self:set_sprite({ animframe_x = 0 })
		end
	end,
	on_button_press = function(self, evt)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()

		local vmx = oak.vs.transform_point(oak.vs.get_id(2), { x = oak.input.mx, y = oak.input.my })

		if evt.action == 0 then
			if self.hover then
				self:set_sprite({ animframe_x = 2 })
			else
				self:set_sprite({ animframe_x = 0 })
			end
		end
		if point_intersects(tc, aabb, vmx) then
			if evt.action ~= 0 then
				self:set_sprite({ animframe_x = 1 })
			end
			self:callback(evt.button, evt.action)
			return true
		end
	end,

	toggle_active = function(self, active)
		if active ~= nil then
			self.active = active
		else
			self.active = not self.active
		end
		if self.active then
			self:set_sprite({ animframe_y = 1 })
		else
			self:set_sprite({ animframe_y = 0 })
		end
	end
}

return { prefab, button }