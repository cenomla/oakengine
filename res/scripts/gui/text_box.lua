local text_field = {
	active = 0,
	on_create = function(self)
		self.text = {}
		self.cursor = oak.es:create_entity(self:layer(), {})
		self.cursor:addTransform({})
		self.cursor:addSprite({
			sprite = hash("spr_gui_cursor")
		})
	end,
	set_active = function(self, active)
		self:setSprite({ animframe_y = active })
		if self.callback ~= nil then 
			self:callback(active)
		end
		self.active = active
		if active == 1 then
			self.cursor:activate()
			local tc = self:getTransform()
			tc.position.z = tc.position.z + 0.1
			self.cursor:setTransform(tc)
		else
			self.cursor:deactivate()
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
		if evt.action ~= 0  and evt.key == 259 and #self.text > 0 then
			table.remove(self.text)
			self:update_text()
		end
	end,
	on_button_press = function(self, evt)
		local tc = self:getTransform()
		local aabb = { half_extent = { x = 112.0, y = 12.0 }, offset = { x = 112.0, y = 12.0 } }

		local sp = self:getSprite()
		if evt.action == 0 then self:setSprite({ animframe_x = 0 }) end
		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
			if evt.action == 0 then
				if sp.animframe_x == 1 then 
					self:set_active(1) 
					if evt.button == 1 then
						self.text = {}
						self:update_text()
					end
				end
			else
				self:setSprite({ animframe_x = 1 })
			end
			return true
		else 
			self:set_active(0)
		end
	end,
	on_char_press = function(self, evt)
		if self.active ~= 0 then
			print(evt.codepoint)
			table.insert(self.text, evt.codepoint)
			self:update_text()
		end
	end,
	update_text = function(self)
		local str = ""
		if #self.text > 0 then str = utf8.char(table.unpack(self.text)) end
		self:setText({ text = str })
	end
}

return text_field