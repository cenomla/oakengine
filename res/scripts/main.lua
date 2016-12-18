package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)


print("hello world")

oak.es:create_entity(0, "player")
oak.es:create_entity(0, "block")

e = oak.es:create_entity(0, "block")
e:setTransform({ position = { x = 512, y = 496 } })

oak.es:create_entity(1, "button")