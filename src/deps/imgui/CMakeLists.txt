cmake_minimum_required(VERSION 3.20)

set(p "sfmlimgui")
project(p)

set(${p}_SOURCES
  sfml.cpp
)

set(${p}_HEADERS
  sfml.hpp
)

find_package(sfml REQUIRED)
find_package(imgui REQUIRED)

add_library(${p} STATIC ${${p}_SOURCES} ${${p}_HEADERS})

target_link_libraries(${p} PRIVATE sfml::sfml imgui::imgui)
