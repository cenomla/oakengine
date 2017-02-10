local console = {
	active = 0,
	on_create = function(self)
		self:deactivate()
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

return console