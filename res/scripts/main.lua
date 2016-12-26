package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)


oak.input = oak.es:create_entity(255, "input")

oak.es:create_entity(0, "player")
oak.es:create_entity(0, "block")

local e = oak.es:create_entity(0, "block")
e:setTransform({ position = { x = 512, y = 496 } })

local te = oak.es:create_entity(2, "gui/tile_editor")

e = oak.es:create_entity(1, "gui/button")
e.callback = function(button) te:on_press(button) end

e = oak.es:create_entity(1, "gui/button")
e:setTransform({ position = { x = 48 }})
