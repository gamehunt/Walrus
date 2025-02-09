cmake_minimum_required(VERSION 3.16)

project(Walrus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(DPP REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

add_executable(${PROJECT_NAME} 
    src/cparse/builtin-features.cpp
    src/cparse/containers.cpp
    src/cparse/functions.cpp
    src/cparse/packToken.cpp
    src/cparse/shunting-yard.cpp
    src/main.cpp
    src/dice.cpp
    src/list.cpp
    src/initiative.cpp
    src/roll_engine.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE include ${DPP_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${DPP_LIBRARIES} nlohmann_json::nlohmann_json)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
