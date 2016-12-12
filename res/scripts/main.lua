package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)


print("hello world")

oak.es:createEntity(0, "player")

local e = oak.es:createEntity(0, "player")
e:setTransform({ position = { x = 256, y = 256 } })

oak.es:createEntity(0, "block")

e = oak.es:createEntity(0, "block")
e:setTransform({ position = { x = 512, y = 496 } })

oak.es:createEntity(1, "button")