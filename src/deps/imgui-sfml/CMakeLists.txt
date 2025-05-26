cmake_minimum_required(VERSION 3.20)
set(p "imgui-sfml")
project(${p})

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



# Пути к SFML (ты указываешь вручную — допустимо, но лучше через find_package, если возможно)
include_directories(/opt/homebrew/Cellar/sfml@2/2.6.2_1/include)
link_directories(/opt/homebrew/Cellar/sfml@2/2.6.2_1/lib)

add_library(${p} STATIC ${${p}_SOURCES} ${${p}_HEADERS})

# 💡 Добавляем OpenGL.framework
find_library(OPENGL_LIBRARY OpenGL)
target_link_libraries(${p} PRIVATE sfml-graphics sfml-window sfml-system ${OPENGL_LIBRARY})
