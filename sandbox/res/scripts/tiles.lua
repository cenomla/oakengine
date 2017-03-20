local tiles = {
	air = {
		dx = 0.0,
		dy = 0.0,
		dw = 0.0,
		dh = 0.0,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 0.0,
		height = 0.0,
		animframe_x = 0,
		animframe_y = 0,
		material = 0,
		flags = 0
	},
	floor = {
		dx = 0.5,
		dy = 0.0,
		dw = 0.1,
		dh = 0.125,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 16.0,
		height = 16.0,
		animframe_x = 0,
		animframe_y = 0,
		material = 1,
		flags = 1
	},
	wall_side = {
		dx = 0.0,
		dy = 0.5,
		dw = 0.1,
		dh = 0.125,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 16.0,
		height = 16.0,
		animframe_x = 0,
		animframe_y = 0,
		material = 2,
		flags = 3
	},
	wall_top = {
		dx = 0.0,
		dy = 0.0,
		dw = 0.1,
		dh = 0.125,
		offset_x = 0.0,
		offset_y = 0.0,
		width = 16.0,
		height = 16.0,
		animframe_x = 0,
		animframe_y = 0,
		material = 3,
		flags = 3
	}

}

return tiles