local game = {

	on_start = function(self, load)
		self.player = oak.es:create_entity(0, "player")
		self.block1 = oak.es:create_entity(0, "block")

		self.block2 = oak.es:create_entity(0, "block")
		if not load then
			self.block2:set_transform({ position = { x = 512, y = 496 } })
		end
		if load then
			oak.save.open("save::save.dat", 2)
			if oak.save.is_open() then
				print("loading save")
				oak.save.close()
			else 
				oak.sm:switch(oak.sm.menu)
			end

			oak.save.open("save::entities/components.dat", 2)
			oak.save.stream_entity(self.player)
			oak.save.stream_entity(self.block1)
			oak.save.stream_entity(self.block2)
			oak.save.close()
		end
	end,

	on_update = function(self)

	end,

	on_end = function(self)
		oak.save.open("save::entities/components.dat", 1)
		oak.save.stream_entity(self.player)
		oak.save.stream_entity(self.block1)
		oak.save.stream_entity(self.block2)
		oak.save.close()
		oak.es:destroy_all()
	end

}

return game