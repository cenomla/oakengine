package.path = "sandbox/res/scripts/?.lua;sandbox/res/scripts/?/init.lua" .. package.path


oak.input = oak.es:create_entity(100000000.0, require("input"))

function load_resources()

oak.load_shader({
	name = "shd_pass",
	path = "core/graphics/shaders/pass2d"
})

oak.load_shader({
	name = "shd_font",
	path = "core/graphics/shaders/font"
})

oak.load_texture({
	name ="tex_font",
	path = "res::fonts/dejavu_sans/atlas",
	flags = 1
})

oak.load_texture({
	name = "tex_light",
	path = "res::textures/light",
	flags = 0
})

oak.load_atlas({
	name = "atlas_entity",
	textures = {
		"res::textures/character", "res::textures/block"
	},
	extent = {
		x = 512,
		y = 512
	},
	flags = 0
})

oak.load_atlas({
	name = "atlas_tiles",
	textures = {
		"res::textures/tiles"
	},
	extent = {
		x = 80,
		y = 64
	},
	flags = 0
})

oak.load_atlas({
	name = "atlas_gui",
	textures = {
		"res::textures/gui/button",
		"res::textures/gui/window",
		"res::textures/gui/title_bar",
		"res::textures/gui/pencil"
	},
	extent = {
		x = 1024,
		y = 1024
	},
	flags = 0
})

oak.load_material({
	name = "mat_entity",
	shader = "shd_pass",
	texture = "atlas_entity"
})

oak.load_material({
	name = "mat_gui",
	shader = "shd_pass",
	texture = "atlas_gui"
})

oak.load_material({
	name = "mat_font",
	shader = "shd_font",
	texture = "tex_font"
})

oak.load_material({
	name = "mat_light",
	shader = "shd_pass",
	texture = "tex_light"
})

oak.set_uniform("shd_font", "text_width", 0.4)
oak.set_uniform("shd_font", "text_edge", 0.4)
oak.set_uniform("shd_font", "text_color", { 1.0, 1.0, 1.0 })
oak.set_uniform("shd_font", "border_width", 0.6)
oak.set_uniform("shd_font", "border_edge", 0.4)
oak.set_uniform("shd_font", "border_color", { 0.0, 0.0, 0.0 })

oak.load_material({
	name = "mat_tiles",
	shader = "shd_pass",
	texture = "atlas_tiles"
})

oak.load_font({
	name = "fnt_dejavu",
	material = "mat_font",
	path = "res::fonts/dejavu_sans"
})

oak.load_sprite({
	name = "spr_player",
	material = "mat_entity",
	center = { x = 8.0, y = 8.0 },
	extent = { x = 16.0, y = 16.0 },
	atlas = "atlas_entity",
	path = "res::textures/character"
})

oak.load_sprite({
	name = "spr_block",
	material = "mat_entity",
	center = { x = 64.0, y = 16.0 },
	extent = { x = 128.0, y = 32.0 },
	atlas = "atlas_entity",
	path = "res::textures/block"
})

oak.load_sprite({
	name = "spr_button",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/button",
	animframes_x = 3,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_pencil",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/pencil",
	animframes_x = 3,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_window",
	material = "mat_gui",
	extent = { x = 1.0, y = 1.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/window"
})

oak.load_sprite({
	name = "spr_title_bar",
	material = "mat_gui",
	extent = { x = 1.0, y = 1.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/title_bar"
})



package.loaded['prefabs/player'] = nil
package.loaded['prefabs/block'] = nil
package.loaded['prefabs/gui/button'] = nil
package.loaded['prefabs/gui/window'] = nil
package.loaded['prefabs/gui/title_bar'] = nil

oak.load_prefab("player", table.unpack(require("prefabs/player")))
oak.load_prefab("block", table.unpack(require("prefabs/block")))
oak.load_prefab("button", table.unpack(require("prefabs/gui/button")))
oak.load_prefab("window", table.unpack(require("prefabs/gui/window")))
oak.load_prefab("title_bar", table.unpack(require("prefabs/gui/title_bar")))

end

load_resources()

oak.vs.define_view(0, { 0, 1 })
oak.vs.define_view(1, { 2 })
oak.vs.set_view(0, { x = 0, y = 0, width = 1280, height = 720 })
oak.vs.set_view(1, { x = 0, y = 0, width = 1280, height = 720 })


--game states
oak.sm = {
	states = {},
	push = function(self, state)
		table.insert(self.states, state)
		state:on_start()
	end,
	pop = function(self)
		if #self.states > 0 then
			self.states[#self.states]:on_end()
			table.remove(self.states)
		end
	end,
	switch = function(self, state, ...)
		--pop all states then switch to new state
		for i = 0, #self.states do
			self:pop()
		end
		if self.current_state ~= nil then
			self.current_state:on_end()
		end
		self.current_state = state
		if state ~= nil then
			state:on_start(...)
		end
	end
}

oak.sm.menu = require("states/menu")
oak.sm.game = require("states/game")
oak.sm:switch(oak.sm.menu)