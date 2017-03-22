package.path = "sandbox/res/scripts/?.lua;sandbox/res/scripts/?/init.lua" .. package.path


oak.input = oak.es:create_entity(255, require("input"))

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
		"res::textures/tiles/geometric_wall_1"
	},
	extent = {
		x = 8000, 
		y = 6400
	},
	flags = 0
})

oak.load_atlas({
	name = "atlas_gui",
	textures = {
		"res::textures/gui/button",
		"res::textures/gui/arrows",
		"res::textures/gui/text_box",
		"res::textures/gui/cursor",
		"res::textures/gui/menu/button",
		"res::textures/gui/tile_editor/background",
		"res::textures/gui/tile_editor/icons",
		"res::textures/gui/tile_editor/selector",
		"res::textures/gui/tool_bar/background",
		"res::textures/gui/tool_bar/icons",
		"res::textures/gui/console/background",
		"res::textures/gui/debug_menu/background"
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
oak.set_uniform("shd_font", "text_color", { 0.0, 0.0, 0.0 })
oak.set_uniform("shd_font", "border_width", 0.0)
oak.set_uniform("shd_font", "border_edge", 0.0)
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
	name = "spr_menu_button",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/menu/button",
	animframes_x = 3,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_gui_button",
	material = "mat_gui",
	center = { x = 0.0, y = 0.0 },
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/button",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_tile_editor",
	material = "mat_gui",
	center = { x = 0.0, y = 0.0 },
	extent = { x = 256.0, y = 512.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/tile_editor/background"
})

oak.load_sprite({
	name = "spr_tile_editor_icons",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/tile_editor/icons",
	animframes_x = 5,
	animframes_y = 4
})

oak.load_sprite({
	name = "spr_gui_arrows",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/arrows",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_gui_text_box",
	material = "mat_gui",
	extent = { x = 224.0, y = 24.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/text_box",
	animframes_x = 2,
	animframes_y = 2
})

oak.load_sprite({
	name = "spr_gui_cursor",
	material = "mat_gui",
	extent = { x = 2.0, y = 18.0 },
	center = { x = -1.0, y = -2.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/cursor"
})

oak.load_sprite({
	name = "spr_tile_editor_tilemap",
	material = "mat_tiles",
	extent = { x = 8000.0, y = 6400.0 },
	draw_position = { x = 0.0, y = 0.0 },
	draw_extent = { x = 1.0, y = 1.0 }
})

oak.load_sprite({
	name = "spr_tile_editor_selector",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	center = { x = 8.0, y = 8.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/tile_editor/selector"
})

oak.load_sprite({
	name = "spr_tool_bar",
	material = "mat_gui",
	extent = { x = 176.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/tool_bar/background"
})

oak.load_sprite({
	name = "spr_tool_bar_icons",
	material = "mat_gui",
	extent = { x = 32.0, y = 32.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/tool_bar/icons",
	animframes_x = 4
})

oak.load_sprite({
	name = "spr_console",
	material = "mat_gui",
	extent = { x = 512.0, y = 256.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/console/background"
})

oak.load_sprite({
	name = "spr_debug_menu",
	material = "mat_gui",
	extent = { x = 256.0, y = 512.0 },
	atlas = "atlas_gui",
	path = "res::textures/gui/debug_menu/background"
})

package.loaded['prefabs/player'] = nil
package.loaded['prefabs/block'] = nil
package.loaded['prefabs/gui/button'] = nil

oak.load_prefab("player", table.unpack(require("prefabs/player")))
oak.load_prefab("block", table.unpack(require("prefabs/block")))
oak.load_prefab("button", table.unpack(require("prefabs/gui/button")))

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