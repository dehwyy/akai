TARGET = Akai

init:
	mkdir build -p && cd build && cmake ..

r:
	cmake --build ./build && ./build/$(TARGET).exe

b:
	cmake --build ./build

conan:
	conan install . --output-folder=build --build=missing
