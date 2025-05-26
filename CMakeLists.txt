cmake_minimum_required(VERSION 3.20)

project(Akai)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к исходникам
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ui/ui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/function.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/variables.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/state/state.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ui/ui.hpp
)

# Указать путь к SFML CMake-модулям (важно!)
set(SFML_DIR "/opt/homebrew/Cellar/sfml@2/2.6.2_1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_subdirectory(src/deps/imgui-sfml)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    imgui-sfml
    sfml-graphics
    sfml-window
    sfml-system
)
