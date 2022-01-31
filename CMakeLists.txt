cmake_minimum_required(VERSION 3.22)

project(pico_read)

find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE INC include/*.hpp)

add_executable(pico_read_main)
target_sources(pico_read_main PRIVATE ${SRC})
target_include_directories(pico_read_main PRIVATE include)

set_property(TARGET pico_read_main PROPERTY CXX_STANDARD 20)
set_property(TARGET pico_read_main PROPERTY CXX_STANDARD_REQUIRED 20)
set_property(TARGET pico_read_main PROPERTY EXPORT_COMPILE_COMMANDS ON)
set_property(TARGET pico_read_main PROPERTY COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Wshadow)

target_link_libraries(pico_read_main nlohmann_json::nlohmann_json xdo)
