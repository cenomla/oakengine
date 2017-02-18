local tile_editor = {
	active = 0,
	on_create = function(self)
		self.children = {}
		self.buttons = {}
		self.tools = {}
		self.num_layers = 0
		self.current_layer = -1
		self.layer_names = {}
		self.layer_depths = {}
		self.tools[1] = function(button, active)
			if active == 1 then
				self.current_layer = oak.ts.create_layer(hash("mat_tiles"))
				self.num_layers = self.num_layers + 1
				self.layer_text:set_text({ text = self.current_layer + 1 .. "/" .. self.num_layers })
				self.layer_name.value = {}
				self.layer_name:update_text()
				self.layer_depth.value = { utf8.codepoint("-1000000.0", 1, 10) }
				self.layer_depth:update_text()
				self.layer_depths[self.current_layer + 1] = "-1000000.0"
				button:set_active(0)
			end
		end

		self.tools[2] = function(button, active)
			if active == 1 then
				if self.num_layers > 0 then
					oak.ts.destroy_layer(self.current_layer)
					table.remove(self.layer_names, self.current_layer + 1)
					table.remove(self.layer_depths, self.current_layer + 1)
					if self.current_layer == self.num_layers - 1 then
						self.current_layer = self.current_layer - 1
					end
					self.num_layers = self.num_layers - 1
					local ld = self.layer_depths[self.current_layer + 1]
					self.layer_depth.value = { utf8.codepoint(ld, 1, #ld) }
					self.layer_depth:update_text()
					local ln = self.layer_names[self.current_layer + 1]
					if ln then
						self.layer_name.value = { utf8.codepoint(ln, 1, #ln) }
					else 
						self.layer_name.value = {}
					end
					self.layer_name:update_text() 
					self.layer_text:set_text({ text = self.current_layer + 1 .. "/" .. self.num_layers })
				end
				button:set_active(0)
			end
		end

		self.tools[5] = function(button, active)
			if active == 1 then
				for k, v in pairs(self.buttons) do	
					if v ~= button then
						v:set_active(0)
					end
				end
				self.tool.update = function(self, dt)
					if oak.input.buttons[0] ~= 0 and self.num_layers > 0 then
						local vmx = oak.vs.transform_point(oak.vs.get_id(0), { x = oak.input.mx, y = oak.input.my })
						local c = oak.ts.get_chunk(math.floor(vmx.x), math.floor(vmx.y), self.current_layer)
						local ttsx = 1.0 / 8000.0
						local ttsy = 1.0 / 6400.0
						local stc = self.selector:get_transform()
						local fs = 0
						if self.children[31].active == 1 then fs = fs | 1 end
						if self.children[33].active == 1 then fs = fs | 2 end

						c:set_tile((vmx.x // 16) % 16, (vmx.y // 16) % 16, {
							dx = (stc.position.x - 16.0) * ttsx,
							dy = (stc.position.y - 192.0) * ttsy,
							dw = ttsx * 16.0,
							dh = ttsy * 16.0,
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
				for k, v in pairs(self.buttons) do	
					if v ~= button then
						v:set_active(0)
					end
				end
				self.tool.update = function(self, dt)
					if oak.input.buttons[0] ~= 0 then
						local vmx = oak.vs.transform_point(oak.vs.get_id(0), { x = oak.input.mx, y = oak.input.my })
						local c = oak.ts.get_chunk(math.floor(vmx.x), math.floor(vmx.y), self.current_layer)
						c:set_tile((vmx.x // 16) % 16, (vmx.y // 16) % 16, {
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
			local e = oak.es:create_entity(self:layer(), "gui_button", require("gui/button"))
			e.callback = function(button, active)
				if self.tools[i + 1] ~= nil then
					self.tools[i + 1](button, active)
				end
			end
			e:set_transform({ 
				position = { 
					x = 16 + ((i % 5) * 48), 
					y = 48 + ((i // 5) * 48)
				} 
			})
			local icon = oak.es:create_entity(self:layer(), {})
			icon:add_transform({
				position = { 
					x = 16 + ((i % 5) * 48), 
					y = 48 + ((i // 5) * 48),
					z = 2.0
				}
			})
			icon:add_sprite({
				sprite = hash("spr_tile_editor_icons"),
				animframe_x = i % 5,
				animframe_y = i // 5
			})

			table.insert(self.buttons, e)
			table.insert(self.children, e)
			table.insert(self.children, icon)
		end
		for i = 0, 4 do
			local e = oak.es:create_entity(self:layer(), "gui_button", require("gui/button"))
			e:set_transform({
				position = {
					x = 16 + (i * 48),
					y = 424.0
				}
			})
			local icon = oak.es:create_entity(self:layer(), {})
			icon:add_transform({
				position = { 
					x = 16 + (i * 48), 
					y = 424.0,
					z = 2.0
				} 
			})
			icon:add_sprite({
				sprite = hash("spr_tile_editor_icons"),
				animframe_x = i,
				animframe_y = 3
			})
			table.insert(self.children, e)
			table.insert(self.children, icon)
		end

		e = oak.es:create_entity(self:layer(), "gui_text_box", require("gui/text_box"))
		e:set_transform({
			position = {
				x = 16.0,
				y = 512.0
			},
			scale = 0.8
		})
		e:set_aabb2d({
			half_extent = { x = 112.0, y = 8.0 },
			offset = { x = 112.0, y = 8.0 }
		})
		e.size = 12.0
		table.insert(self.children, e)



		self.tilemap = oak.es:create_entity(self:layer(), {})
		self.tilemap:add_transform({
			position = {
				x = 16.0,
				y = 192.0, 
				z = 1.0
			}
		})
		self.tilemap:add_sprite({
			sprite = hash("spr_tile_editor_tilemap")
		})
		table.insert(self.children, self.tilemap)

		self.selector = oak.es:create_entity(self:layer(), {})
		self.selector:add_transform({
			position = {
				x = 16.0,
				y = 192.0,
				z = 5.0
			}
		})
		self.selector:add_sprite({
			sprite = hash("spr_tile_editor_selector")
		})
		table.insert(self.children, self.selector)



		--layer selector 	
		self.layer_text = oak.es:create_entity(self:layer(), {})
		self.layer_text:add_transform({
			position = {
				x = 184.0,
				y = 464.0,
				z = 2.0
			}
		})
		self.layer_text:add_text({
			font = hash("fnt_dejavu"),
			size = 12.0,
			offset = { x = 2.0, y = 2.0 },
			text = self.current_layer + 1 .. "/" .. self.num_layers
		})
		table.insert(self.children, self.layer_text)

		self.layer_name = oak.es:create_entity(self:layer(), "gui_text_box", require("gui/text_box"))
		self.layer_name:remove_sprite()
		self.layer_name:set_transform({
			position = {
				x = 32.0,
				y = 464.0,
				z = 1.5
			},
			scale = 0.8
		})
		self.layer_name:set_aabb2d({
			half_extent = { x = 56.0, y = 8.0 },
			offset = { x = 56.0, y = 8.0 }
		})
		self.layer_name.callback = function(e, active)
			if active == 0 then
				self.layer_names[self.current_layer + 1] = utf8.char(table.unpack(e.value))
			end
		end
		table.insert(self.children, self.layer_name)

		self.layer_depth = oak.es:create_entity(self:layer(), "gui_text_box", require("gui/text_box"))
		self.layer_depth:remove_sprite()
		self.layer_depth:set_transform({
			position = {
				x = 16.0,
				y = 488.0,
				z = 1.5
			},
			scale = 0.8
		})
		self.layer_depth:set_aabb2d({
			half_extent = { x = 112.0, y = 8.0 },
			offset = { x = 112.0, y = 8.0 }
		})
		self.layer_depth.callback = function(e, active)
			if active == 0 then
				self.layer_depths[self.current_layer + 1] = utf8.char(table.unpack(e.value))
				oak.ts.set_layer_depth(self.current_layer, tonumber(self.layer_depths[self.current_layer+1]))
			end
		end
		table.insert(self.children, self.layer_depth)

		local lau = oak.es:create_entity(self:layer(), "gui_arrow", require("gui/button"))
		lau:set_transform({
			position = {
				x = 32.0,
				y = 480.0,
				z = 2.0
			},
			scale = -0.5
		})
		lau:add_aabb2d({
			half_extent = { x = 8.0, y = 8.0 },
			offset = { x = -8.0, y = -8.0 }
		})
		lau.callback = function(arrow, active)
			if active == 1 then
				if self.current_layer < self.num_layers - 1 then
					self.current_layer = self.current_layer + 1 
					self.layer_text:set_text({ text = self.current_layer + 1 .. "/" .. self.num_layers })
				end
				arrow:set_active(0)
				local ld = self.layer_depths[self.current_layer + 1]
				self.layer_depth.value = { utf8.codepoint(ld, 1, #ld) }
				self.layer_depth:update_text()
				local ln = self.layer_names[self.current_layer + 1]
				if ln then
					self.layer_name.value = { utf8.codepoint(ln, 1, #ln) }
				else 
					self.layer_name.value = {}
				end
				self.layer_name:update_text() 
			end
		end
		local lad = oak.es:create_entity(self:layer(), "gui_arrow", require("gui/button"))
		lad:set_transform({
			position = {
				x = 144.0,
				y = 464.0,
				z = 2.0
			},
			scale = 0.5
		})
		lad.callback = function(arrow, active)
			if active == 1 then
				if self.current_layer > 0 then
					self.current_layer = self.current_layer - 1
					self.layer_text:set_text({ text = self.current_layer + 1 .. "/" .. self.num_layers })
				end
				arrow:set_active(0)
				local ld = self.layer_depths[self.current_layer + 1]
				self.layer_depth.value = { utf8.codepoint(ld, 1, #ld) }
				self.layer_depth:update_text()
				local ln = self.layer_names[self.current_layer + 1]
				if ln then
					self.layer_name.value = { utf8.codepoint(ln, 1, #ln) }
				else 
					self.layer_name.value = {}
				end
				self.layer_name:update_text() 
			end
		end
		table.insert(self.children, lau)
		table.insert(self.children, lad)

		--tool table
		self.tool = {}
		--deactivate
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
		local tc = self.tilemap:get_transform()
		local aabb = { half_extent = { x = 112.0, y = 112.0 }, offset = { x = 112.0, y = 112.0 } }

		if point_intersects(tc, aabb, { x = oak.input.mx, y = oak.input.my }) then
			if evt.button == 0 then
				if evt.action == 0 then
					local tx = (oak.input.mx - 16) // 16
					local ty = (oak.input.my - 192) // 16
					self.selector:set_transform({
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

	toggle_active = function(self, active)
		if active ~= self.active then
			if self.active == 0 then
				self.active = 1
				self:activate()
			else
				self.active = 0
				self:deactivate()
			end
		end
	end

}

return tile_editor