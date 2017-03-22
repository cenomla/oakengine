require("util/io")
local game = {

	on_start = function(self, load)
		self.player = oak.es:create_entity(0, "player")
		self.p = oak.es:create_entity(0, "player")
		self.p.max_speed = 400.0
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
				print("save doesnt exist")
				oak.sm:switch(oak.sm.menu)
			end

			oak.save.open("save::entities/components.dat", 2)
			oak.save.stream_entity(self.player)
			oak.save.stream_entity(self.block1)
			oak.save.stream_entity(self.block2)

			load_table("sandbox/save/entities/player.lua", self.player)
			load_table("sandbox/save/entities/block1.lua", self.block1)
			load_table("sandbox/save/entities/block2.lua", self.block2)

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

		save_table("sandbox/save/entities/player.lua", self.player)
		save_table("sandbox/save/entities/block1.lua", self.block1)
		save_table("sandbox/save/entities/block2.lua", self.block2)
		
		oak.es:destroy_all()
	end

}

return game