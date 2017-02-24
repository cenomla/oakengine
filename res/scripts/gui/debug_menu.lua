local debug_menu = {
	on_create = function(self)
		self.text = oak.es:create_entity(self:layer(), {})
		self.text:add_transform({
			position = {
				x = 16.0,
				y = 48.0
			}
		})
		self.text:add_text({
			font = hash("fnt_dejavu"),
			text = "debug test",
			size = 12.0
		})
		self:deactivate()
	end,
	on_update = function(self)
		local dvars = oak.debug_get_vars()
		local str = "dt: " .. dvars.delta_time .. " fps: " .. dvars.fps .. " used mem: " .. dvars.used_memory .. " allocated mem: " .. dvars.allocated_memory

		self.text:set_text({
			text = str
		})
	end,
	on_activate = function(self)
		self.text:activate()
	end,
	on_deactivate = function(self)
		self.text:deactivate()
	end,
	toggle_active = function(self, active)
		if active ~= self:is_active() then
			if self:is_active() then
				self:deactivate()
			else
				self:activate()
			end
		end
	end
}

return debug_menu