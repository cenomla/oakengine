require("util/io")
local game = {

	on_start = function(self, load)
		self.window = oak.es:create_entity(2, "window")
		table.insert(self.window.children, make_button(2, 0, 0, "spr_button", 128, 32, "Reload", 
			function(b, button, action) if action == 0 then load_resources() end end))
		table.insert(self.window.children, make_button(2, 0, 0, "spr_button", 96, 32, "Save",
			function(b, button, action) if action == 0 then self:save() end end))
		table.insert(self.window.children, make_button(2, 0, 0, "spr_button", 96, 32, "Load",
			function(b, button, action) if action == 0 then self:load() end end))
		table.insert(self.window.children, make_button(2, 0, 0, "spr_button", 128, 32, "Pause"))
		table.insert(self.window.children, make_button(2, 0, 0, "spr_button", 96, 32, "Exit",
			function(b, button, action) if action == 0 then oak.sm:switch(oak.sm.menu) end end))
		self.window:update_children()
		self.player = oak.es:create_entity(0, "player")
		self.block1 = oak.es:create_entity(0, "block")
		self.block2 = oak.es:create_entity(0, "block")
		if load then
			self:load()
		else
			self.block2:set_transform({ position = { x = 512, y = 496 } }) 
		end
	end,

	on_update = function(self)

	end,

	on_end = function(self)
		self:save()
		oak.es:destroy_all()
	end,
	save = function(self)
		oak.save.open("save::entities/components.dat", 1)
		oak.save.stream_entity(self.player)
		oak.save.stream_entity(self.block1)
		oak.save.stream_entity(self.block2)
		oak.save.stream_entity(self.window)
		oak.save.close()

		save_table("sandbox/save/entities/player.lua", self.player)
		save_table("sandbox/save/entities/block1.lua", self.block1)
		save_table("sandbox/save/entities/block2.lua", self.block2)
	end,

	load = function(self)
		oak.save.open("save::save.dat", 2)
		if oak.save.is_open() then
			print("loading save")
			oak.save.close()
		else 
			print("save doesnt exist")
			return
		end

		oak.save.open("save::entities/components.dat", 2)
		oak.save.stream_entity(self.player)
		oak.save.stream_entity(self.block1)
		oak.save.stream_entity(self.block2)
		oak.save.stream_entity(self.window)
		oak.save.close()

		load_table("sandbox/save/entities/player.lua", self.player)
		load_table("sandbox/save/entities/block1.lua", self.block1)
		load_table("sandbox/save/entities/block2.lua", self.block2)
		self.window:update_children()

	end

}

return game