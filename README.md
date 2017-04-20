# Oak Engine
### Overview:
 
Oak engine is small game engine aimed at being lightweight, fast and extensible that is still in very early stages. Oak engine is written using modern c++ and uses lua as a scripting language, I plan to add bindings for javascript and possibly c# in the future. The engine is designed to use multi-threading at its core and will support vulkan to take full advantage of this. The sandbox folder contains a small demo detailing the use of the engine and a few additional systems. The goal of this engine is not to provide everything a game may need (physically based rendering, rigid body physics, etc..), as so many librarys do these things already, but rather to provide a solid framework to build a game off of and extend with additional functionality.

### Features:
- Multi-threaded task manager
- Event/Messaging system
- Efficent memory allocation system
- Component based entity system
- Lua scripting
- Prefab system with shared components
- Simple opengl sprite batcher
- Easily extendable

### Building:
 
Oak engine depends on glfw, glad, glm, lua, enet, and stb. SConstruct is used for the build process.

##### Linux instructions:
~~~~
sudo apt-get install scons
~~~~
Or your distros equivalent command.

Then from there you just cd to the directory where the source is located and
~~~~
scons
~~~~
or
~~~~
scons debug | release
~~~~
for flavor specific builds.

##### Windows/Mac:
 
I currently don't have access to one of these platform for testing the build on so I dont have a build system for them. However Oak engine is coded using completely crossplatform code and librarys so it should build without too much trouble. 

### Documentation:

Coming soon (for now see the main.cpp file in the sandbox folder) 
