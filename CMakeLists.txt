cmake_minimum_required(VERSION 3.10)
project(HeightOMatic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Modbus (local build)
set(MODBUS_INCLUDE_DIR ~/libmodbus/src)
set(MODBUS_LIBRARY ~/libmodbus/src/.libs/libmodbus.so)

# SDL2
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

add_executable(heightmatic main.cpp utils.cpp device-handler.cpp)

target_include_directories(heightmatic PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${MODBUS_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(heightmatic PRIVATE 
    ${MODBUS_LIBRARY}
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    pthread
)
