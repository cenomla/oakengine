function make_button(depth, x, y, sprite, width, height, text, callback)
	local b = oak.es:create_entity(depth, "button")
	b:set_transform({ position = { x = x, y = y } })
	b:set_sprite({ sprite = hash(sprite) })
	b:set_aabb2d({ offset = { x = width / 2, y = height / 2 }, half_extent = { x = width / 2, y = height / 2 } })
	if text ~= nil then
		b:add_text({
			text = text,
			font = hash("fnt_dejavu"),
			size = height / 2,
			offset = { x = 32.0, y = 6.0 },
			layer = 2
		})
	end
	if callback ~= nil then
		b.callback = callback
	end
	return b
end

function make_window(depth, x, y, width, height, title)
	local w = oak.es:create_entity(depth, "window")
	w:set_transform({ position = { x = x, y = y } })
	w:set_sprite({ scale = { x = width, y = height } })
	if title ~= nil then
		w.title:add_text({
			text = title,
			font = hash("fnt_dejavu"),
			size = 12.0,
			offset = { x = 4.0, y = 2.0 },
			layer = 2
		})
	end
	w:update_children()
	return w
end

function make_tile_editor(window)
	local tile_pencil = oak.es:create_entity(1000.0, {
		on_create = function(self)
			self:deactivate()
		end,
		on_update = function(self, dt)
			if oak.input.buttons[0] ~= 0 then
				oak.ts:get_chunk()
			end
		end
	})


	--add buttons
	table.insert(window.children, make_button(0.0, 0.0, 0.0, "spr_pencil", 32.0, 32.0, nil,
		function(b, button, action)
			if button == 0 and action == 0 then
				b:toggle_active()
			end
			if b.active then --enable tile placement tool
				tile_pencil:activate()
			else
				tile_pencil:deactivate()
			end
		end))
	window:update_children()

	return window
end