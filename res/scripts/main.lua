package.path = "res/scripts/?.lua;res/scripts/?/init.lua" .. package.path
print(package.path)


print("hello world")

oak.es:createEntity(0, "player")

local e = oak.es:createEntity(0, "player")
e:setPosition(256, 256)

oak.es:createEntity(0, "block")
e = oak.es:createEntity(0, "block")

e:setPosition(512, 496)