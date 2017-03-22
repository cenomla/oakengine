local menu = {

	on_start = function(self)
		local e = oak.es:create_entity(0, "button")
		e:set_transform({position = { x = 128.0, y = 128.0 }})
		e.callback = function(b, button, action)
			oak.sm:switch(oak.sm.game)
		end
	end,

	on_update = function(self)

	end,

	on_end = function(self)
		oak.es:destroy_all()
	end

}

return menu