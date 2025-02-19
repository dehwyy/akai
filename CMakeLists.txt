cmake_minimum_required(VERSION 3.20)

project(CSProject)


# set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)


include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(nlohmann_json REQUIRED)


add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
# add_custom_target(
#     clang-format
#     COMMAND clang-format
#     --dry-run
#     -Werror
#     ${SOURCES}
#     ${HEADERS}
# )

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
