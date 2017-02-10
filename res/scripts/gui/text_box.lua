local text_box = {
	active = 0,
	size = 12.0,
	on_create = function(self)
		self.value = {}
		self.text = {}
		self.cursor = oak.es:create_entity(self:layer(), {})
		self.cursor:add_transform({})
		self.cursor:add_sprite({
			sprite = hash("spr_gui_cursor")
		})
	end,
	set_active = function(self, active)
		self:set_sprite({ animframe_y = active })
		self.active = active
		if active == 1 then
			self.text = {}
			self.cursor:activate()
		else
			self.cursor:deactivate()
		end
		self:update_text()
		if self.callback ~= nil then 
			self:callback(active)
		end
	end,
	toggle_active = function(self)
		if self.active == 0 then
			self:set_active(1)
		else 
			self:set_active(0)
		end
	end,
	on_key_press = function(self, evt)
		if self.active == 1 and evt.action ~= 0 then
			if evt.key == 259 and #self.text > 0 then
				table.remove(self.text)
				self:update_text()
			end
			if evt.key == 257 and #self.text > 0 then
				self.value = { table.unpack(self.text) }
				self:set_active(0)
			end
		end
	end,
	on_button_press = function(self, evt)
		local tc = self:get_transform()
		local aabb = self:get_aabb2d()

		local sp = self:get_sprite()
		if evt.action == 0 then self:set_sprite({ animframe_x = 0 }) end
		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
			if evt.action == 0 then
				if sp.animframe_x == 1 then 
					self:toggle_active()
					return true
				end
			else
				self:set_sprite({ animframe_x = 1 })
			end
		else 
			self:set_active(0)
		end
	end,
	on_char_press = function(self, evt)
		if self.active ~= 0 then
			table.insert(self.text, evt.codepoint)
			self:update_text()
		end
	end,
	update_text = function(self)
		local tc = self:get_transform()
		tc.position.x = tc.position.x + #self.text * 9.2
		tc.position.z = tc.position.z + 0.1
		self.cursor:set_transform(tc)
		if self.active == 1 then
			local str = ""
			if #self.text > 0 then str = utf8.char(table.unpack(self.text)) end
			self:set_text({ size = self.size, text = str })
		else 
			local str = ""
			if #self.value > 0 then str = utf8.char(table.unpack(self.value)) end
			self:set_text({ size = self.size, text = str })
		end
	end
}

return text_box