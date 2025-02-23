cmake_minimum_required(VERSION 3.20)

set(p "imgui-sfml")
project(p)

set(${p}_SOURCES
  imgui.cpp
  imgui-sfml.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui_spectrum.cpp
)

set(${p}_HEADERS
  imgui.h
  imgui-sfml.hpp
  imconfig.h
  imgui_internal.h
  imstb_textedit.h
  imstb_truetype.h
  imgui_spectrum.h
)

find_package(sfml REQUIRED)
# find_package(imgui REQUIRED)

add_library(${p} STATIC ${${p}_SOURCES} ${${p}_HEADERS})

target_link_libraries(${p} PRIVATE sfml::sfml)
