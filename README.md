# Oak Engine
### Overview:
  Oak engine is small game engine aimed at being lightweight, fast and extensible that is still in very early stages. Oak engine uses modern c++ (currently c++14 with the use of nested namespaces) with lua as a scripting language, I plan to add bindings for javascript and possibly c# in the future. The engine is designed to use multi-threading at its core and will support vulkan to take full advantage of this. The sandbox folder contains a small test demo detailing the use of the engine and a very simple physics system.

### Features:
  -Multi-threaded task manager
  -Event/Messaging system
  -Custom memory allocation
  -Component based entity system
  -Script binding for lua
  -Simple opengl sprite batcher
  -Easily extendable
  -Can render a triangle using vulkan

### Building:
  Oak engine relies on glfw, glad, glm, lua, enet, and the stb_image.h header that are inside the lib and lib/include folders to compile. On linux it should be as simple as running the SConstruct script.

#####Linux instructions:
~~~~
sudo apt-get install scons
~~~~
Or your distros equivalent command.

Then from there you just cd to the root directory where Oak engine is located and
~~~~
scons
~~~~

#####Windows/Mac:
I currently don't have access to one of these platform for testing the build on so I dont have a build system for them. However Oak engine is coded using completely crossplatform code and librarys so it should build without too much trouble.

### Documentation:
  Coming soon (for now see the main.cpp file in the sandbox folder)
