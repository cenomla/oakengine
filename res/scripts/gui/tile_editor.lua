local tile_editor_map = {
	create = function(self)
		--16 = visible, 17 = solid
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
	end,
	button_press = function(self, evt)
		local tc = self:getTransform()
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
		else
			if evt.button == 0 then
				if evt.action ~= 0 then
					local c = oak.ts:get_chunk(oak.input.mx, oak.input.my, 0)
					local tts = 1.0 / 128.0
					local stc = self.selector:getTransform()
					c:set_tile((oak.input.mx // 16) % 16, (oak.input.my // 16) % 16, {
						dx = (stc.position.x - 16.0) * tts * 0.5,
						dy = (stc.position.y - 192.0) * tts * 0.5,
						dw = tts * 8.0,
						dh = tts * 8.0,
						width = 16.0,
						height = 16.0,
						flags = 1
					})
				end
			end
		end
	end,
	on_activate = function(self)
		self.selector:activate()
	end,
	on_deactivate = function(self)
		self.selector:deactivate()
	end
}

local tile_editor = {

	create = function(self)
		self.children = {}
		self.buttons = {}
		for i = 0, 14 do			
			local e = oak.es:create_entity(1, "gui_button", require("gui/button"))
			e.callback = function(button) 
				for k, v in pairs(self.buttons) do
					if v ~= button then
						v:set_active(0)
					end
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

		local e = oak.es:create_entity(1, require("gui/button"))
		e:addTransform({
			position = {
				x = 208.0,
				y = 464.0,
				z = 1.0
			}
		})
		e:addSprite({
			sprite = hash("spr_gui_arrows")
		})
		table.insert(self.children, e)

		e = oak.es:create_entity(1, require("gui/button"))
		e:addTransform({
			position = {
				x = 208.0,
				y = 504.0,
				z = 1.0
			}
		})
		e:addSprite({
			sprite = hash("spr_gui_arrows")
		})
		table.insert(self.children, e)

		e = oak.es:create_entity(1, tile_editor_map)
		e:addTransform({
			position = {
				x = 16.0,
				y = 192.0, 
				z = 1.0
			}
		})
		e:addSprite({
			sprite = hash("spr_tile_editor_tilemap")
		})
		e.parent = self
		table.insert(self.children, e)

		self:deactivate()
	end,

	update = function(self)

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

	toggle_active = function(self, button)
		if button.active == 1 then self:activate() else self:deactivate() end
	end

}

return tile_editor