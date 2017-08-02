# Oak Engine
### Overview:
 
Oak engine is small game engine aimed at being lightweight, fast and extensible and is still in it's very early stages. Oak engine is written using modern c++ and uses lua as a scripting language, I plan to add bindings for javascript and possibly c# in the future. The engine is designed to use multi-threading at its core and will support vulkan to take full advantage of this. The sandbox folder contains a small demo detailing the use of the engine and a few additional systems.

### Features:
- Multi-threaded task manager
- Event/Messaging system
- Efficent memory allocation management
- Component based entity system
- Lua scripting
- Prefabs with shared components
- Opengl 4 renderer
- Easily extendable

### Libraries Used:

| Library | Site |
| --- | --- |
| glfw | www.glfw.org |
| glad | www.github.com/Dav1dde/glad |
| glm | www.glm.g-truc.net |
| lua | www.lua.org |
| enet | www.enet.bespin.org |
| stb | www.github.com/nothings/stb |
| oail | www.assimp.sourceforge.net |

### Building:

Oak engine uses the meson build system

##### Linux instructions:
~~~~
sudo apt-get install meson
~~~~
Or your distros equivalent command.

Then from there you just cd to the directory where the source is located and
~~~~
mkdir build
cd build
meson ..
ninja
~~~~

##### Windows/Mac:
 
I currently don't have access to one of these platform for testing the build on so I dont have the build system working for them. However Oak engine is coded using completely crossplatform code and librarys so it should build without too much trouble. 

### Documentation:

Coming soon (for now see the main.cpp file in the sandbox folder) 
