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

local menu = {

	on_start = function(self)
		make_button(0.0, 128.0, 128.0, "spr_button", 160.0, 32.0, "Start Game",
		function(b, button, action)
			if action == 0 then
				oak.sm:switch(oak.sm.game, false)
			end
		end)
		make_button(0.0, 128.0, 180.0, "spr_button", 160.0, 32.0, "Load Game", 
		function(b, button, action)
			if action == 0 then
				oak.sm:switch(oak.sm.game, true)
			end
		end)
		make_button(0.0, 128.0, 232.0, "spr_button", 160.0, 32.0, "Settings")
		make_button(0.0, 128.0, 284.0, "spr_button", 160.0, 32.0, "Quit Game",
		function(b, button, action)
			if action == 0 then
				oak.sm:switch(nil)
				oak.quit()
			end
		end)
	end,

	on_update = function(self)

	end,

	on_end = function(self)
		oak.es:destroy_all()
	end

}

return menu