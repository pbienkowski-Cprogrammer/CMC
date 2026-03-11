### CVoxelEngine

My voxel engine project written entirety in C.

Compiling using CMake
---------
For now I tested it on Linux, and i don't know if program works on Windows or macOS.

To compile program on Debian you need some dependencies, to install them run in terminal:
```
sudo apt-get update && sudo apt-get install -y cmake build-essential libglfw3-dev
```
To compile project perform following steps:
1. `cd` to the directory where you unpacked the CVoxelEngine source
   archive, i.e. to the directory containing `src` and `res`.
   
2. Create and change to a new, empty build directory by running `mkdir
   build` and then `cd build`.
   
3. Run `cmake ..` to create the Makefiles needed to build CVoxelEngine.

4. Type `cmake --build .` to start the build process.

5. At this point you should have runnable ./CVoxelEngine in upper directory.
