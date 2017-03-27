require("util/factory")
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