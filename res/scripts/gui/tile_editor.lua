local tile_editor = {

	on_create = function(self)
		self.children = {}
		self.buttons = {}
		self.tools = {}
		self.tools[5] = function(button, active)
			if active == 1 then
				self.tool.update = function(self, dt)
					if oak.input.buttons[0] ~= 0 then
						local c = oak.ts:get_chunk(oak.input.mx, oak.input.my, 0)
						local tts = 1.0 / 128.0
						local stc = self.selector:getTransform()
						local fs = 0
						if self.children[31].active == 1 then fs = fs | 1 end
						if self.children[33].active == 1 then fs = fs | 2 end
						c:set_tile((oak.input.mx // 16) % 16, (oak.input.my // 16) % 16, {
							dx = (stc.position.x - 16.0) * tts * 0.5,
							dy = (stc.position.y - 192.0) * tts * 0.5,
							dw = tts * 8.0,
							dh = tts * 8.0,
							width = 16.0,
							height = 16.0,
							flags = fs
						})
					end
				end
			else 
				self.tool.update = nil
			end
		end

		self.tools[15] = function(button, active)
			if active == 1 then
				self.tool.update = function(self, dt)
					if oak.input.buttons[0] ~= 0 then
						local c = oak.ts:get_chunk(oak.input.mx, oak.input.my, 0)
						c:set_tile((oak.input.mx // 16) % 16, (oak.input.my // 16) % 16, {
							dx = 0.0,
							dy = 0.0,
							dw = 0.0,
							dh = 0.0,
							width = 0.0,
							height = 0.0,
							flags = 0
						})
					end
				end
			else
				self.tool.update = nil
			end
		end

		for i = 0, 14 do			
			local e = oak.es:create_entity(1, "gui_button", require("gui/button"))
			e.callback = function(button, active)
				for k, v in pairs(self.buttons) do	
					if active == 1 and v ~= button then
						v:set_active(0)
					end
				end
				if self.tools[i + 1] ~= nil then
					self.tools[i + 1](v, active)
				end
			end
			e:setTransform({ 
				position = { 
					x = 16 + ((i % 5) * 48), 
					y = 48 + ((i // 5) * 48)
				} 
			})
			local icon = oak.es:create_entity(1, {})
			icon:addTransform({
				position = { 
					x = 16 + ((i % 5) * 48), 
					y = 48 + ((i // 5) * 48),
					z = 2.0
				}
			})
			icon:addSprite({
				sprite = hash("spr_tile_editor_icons"),
				animframe_x = i % 5,
				animframe_y = i // 5
			})

			table.insert(self.buttons, e)
			table.insert(self.children, e)
			table.insert(self.children, icon)
		end
		for i = 0, 4 do
			local e = oak.es:create_entity(1, "gui_button", require("gui/button"))
			e:setTransform({
				position = {
					x = 16 + (i * 48),
					y = 424.0
				}
			})
			local icon = oak.es:create_entity(1, {})
			icon:addTransform({
				position = { 
					x = 16 + (i * 48), 
					y = 424.0,
					z = 2.0
				} 
			})
			icon:addSprite({
				sprite = hash("spr_tile_editor_icons"),
				animframe_x = i,
				animframe_y = 3
			})
			table.insert(self.children, e)
			table.insert(self.children, icon)
		end

		local e = oak.es:create_entity(1, "gui_arrow", require("gui/button"))
		e:setTransform({
			position = {
				x = 208.0,
				y = 464.0
			}
		})
		table.insert(self.children, e)

		e = oak.es:create_entity(1, "gui_text_box", require("gui/text_box"))
		e:setTransform({
			position = {
				x = 16.0,
				y = 504.0
			}
		})
		table.insert(self.children, e)



		self.tilemap = oak.es:create_entity(1, {})
		self.tilemap:addTransform({
			position = {
				x = 16.0,
				y = 192.0, 
				z = 1.0
			}
		})
		self.tilemap:addSprite({
			sprite = hash("spr_tile_editor_tilemap")
		})
		table.insert(self.children, self.tilemap)

		self.selector = oak.es:create_entity(1, {})
		self.selector:addTransform({
			position = {
				x = 16.0,
				y = 192.0,
				z = 5.0
			}
		})
		self.selector:addSprite({
			sprite = hash("spr_tile_editor_selector")
		})
		table.insert(self.children, self.selector)
		self.tool = {}

		self:deactivate()
	end,

	on_update = function(self, dt)
		if oak.input.mx < 256 and oak.input.my < 544 then
			oak.input.buttons[0] = 0
		end
		if self.tool.update ~= nil then
			self.tool.update(self, dt)
		end
	end,

	on_button_press = function(self, evt)
		local tc = self.tilemap:getTransform()
		local aabb = { half_extent = { x = 112.0, y = 112.0 }, offset = { x = 112.0, y = 112.0 } }

		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
			if evt.button == 0 then
				if evt.action == 0 then
					local tx = (oak.input.mx - 16) // 16
					local ty = (oak.input.my - 192) // 16
					self.selector:setTransform({
						position = {
							x = 16.0 + tx * 16.0,
							y = 192.0 + ty * 16.0
						}
					})
				end
				return true
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
	end,

	toggle_active = function(self)
		if not self:is_active() then self:activate() else self:deactivate() end
	end

}

return tile_editor