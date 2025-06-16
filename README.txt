This build environment requires Nix with flake support.
You may choose to either use
    gcc4ti  [a fork built from src, default]
    tigcc   [i686 binary, original software]

We have a beautiful devShell available to make things easy. (thanks to my friend teaching me about Nix!)
You can build the project with make tools, and debug and test with tiemu, all available in the devShell.

Yes, even clangd works, although its not great! (some errors in places, I tried to suppress with .clangd)

Sorry, but VSCode CMake Extension doesn't really work. Maybe someone can get it working?
CMake itself works fine in the command line...

Remember: CMake Rebuild command must specify CMAKE_TOOLCHAIN_FILE.
This should be good for vscode:
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

Known bugs:

    Ninja fucks everything up because tigcc's frontend is garbage that doesn't handle dependency files
    Also Cmake's ninja generator doesn't really know how to respect set(CMAKE_DEPENDS_USE_COMPILER OFF)