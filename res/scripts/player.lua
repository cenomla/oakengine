local player = {
	transform = {
		position = { 128.0, 128.0, 16.0 },
		scale = 1.0,
		rotationAxis = { 0.0, 0.0, 0.0 },
		rotationAngle = 0.0
	},
	sprite = {
		resource = oak.resource.getSprite("player"),
		animFrameX = 5,
		animFrameY = 5
	},
	speed = 400,
	create = function(self)
	end,
	update = function(self, dt)
		local x, y = self:getPosition()
		if oak.input.keyPressed(oak.input.key_w) then
			y = y - self.speed * dt
		end
		if oak.input.keyPressed(oak.input.key_a) then
			x = x - self.speed * dt
		end
		if oak.input.keyPressed(oak.input.key_s) then
			y = y + self.speed * dt
		end
		if oak.input.keyPressed(oak.input.key_d) then
			x = x + self.speed * dt
		end
		self:setPosition(x, y)
	end
}

return player