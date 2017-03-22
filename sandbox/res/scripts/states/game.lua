local game = {

	on_start = function(self)
		oak.es:create_entity(0, "player")
		oak.es:create_entity(0, "block")

		local e = oak.es:create_entity(0, "block")
		e:set_transform({ position = { x = 512, y = 496 } })
	end,

	on_update = function(self)

	end,

	on_end = function(self)
		oak.es:destroy_all()
	end

}

return game