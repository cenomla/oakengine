local input = {

	on_create = function(self)
		self.mx = 0
		self.my = 0
		self.buttons = {}
		for i = 0, 31 do
			self.buttons[i] = 0
		end
		self.keys = {}
		for i = 0, 1024 do
			self.keys[i] = 0
		end
	end,

	on_key_press = function(self, evt)
		self.keys[evt.key] = evt.action
		if evt.key == 82 and evt.action == 0 then
			load_resources()
		end
	end,

	on_button_press = function(self, evt)
		self.buttons[evt.button] = evt.action
	end,

	on_mouse_move = function(self, evt)
		self.mx = evt.x
		self.my = evt.y
	end

}

return input