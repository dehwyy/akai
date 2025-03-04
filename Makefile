TARGET = CSProject

init:
	mkdir build -p && cd build && cmake ..

r:
	cmake --build ./build && ./build/$(TARGET).exe

conan:
	conan install . --output-folder=build --build=missing -s compiler.libcxx=libstdc++11
