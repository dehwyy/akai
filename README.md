# Akai


## Functions' graphic visualizer

### Requirements:

- [Conan](https://conan.io)
- CMake
- Any C++ compiler

### Building
```sh
mkdir build                                                                           #
conan install . --output-folder=build --build=missing -s compiler.libcxx=libstdc++11  # install conan deps
cd build && cmake ..                                                                  # init CMake
cmake --build build                                                                   # build
./build/<output.file>                                                                 # run app
```

## Resources
- [ImGui](https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html)
- [SFML](https://www.sfml-dev.org/documentation/2.6.2/)
