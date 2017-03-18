local console = {
	on_create = function(self)
		self:deactivate()
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

return console