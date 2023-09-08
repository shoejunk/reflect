# reflect
Create a build folder from the root directory if it doesn't exist:
- "mkdir build"

How to generate project files:
- Run "cmake .." from the build directory.

How to build the project:
- Run "cmake --build ." from the build directory.
The library will be located in "build\Deubg\Reflect.lib"

How to generate and build the standalone project:
- cmake -S standalone -B build/standalone
- cmake --build build/standalone
The executable will be located in "build\Debug\Reflect.exe"
