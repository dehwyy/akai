cmake_minimum_required(VERSION 3.20)

project(Akai)

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/state/state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ui/ui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/logger.cpp

)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/function/variables.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/state/state.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ui/ui.hpp
)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(nlohmann_json REQUIRED)
find_package(sfml REQUIRED)
# find_package(imgui REQUIRED)


add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src/deps/imgui-sfml)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json sfml::sfml  imgui-sfml)
