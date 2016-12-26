local input = {

	create = function(self)
		self.mx = 0
		self.my = 0
		self.buttons = {}
		for i = 0, 31 do
			self.buttons[i] = 0
		end
		self.keys = {}
		for i = 0, 255 do
			self.keys[i] = 0
		end
	end,

	key_press = function(self, evt)
		self.keys[evt.key] = evt.action
	end,

	button_press = function(self, evt)
		self.buttons[evt.button] = evt.action
	end,

	mouse_move = function(self, evt)
		self.mx = evt.x
		self.my = evt.y
	end

}

return input