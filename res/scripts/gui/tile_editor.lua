local tile_editor = {

	create = function(self)
		self.children = {}
		for i = 0, 5 do			
			local e = oak.es:create_entity(2, "gui/button")
			e:setTransform({ position = { x = 16 + ((i % 3) * 64), y = 64 + ((i // 3) * 64) }})
			self.children[i] = e
		end
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

	on_press = function(self, button)
		if button.active == 1 then self:activate() else self:deactivate() end
	end

}

return tile_editor