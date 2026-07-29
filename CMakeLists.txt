cmake_minimum_required(VERSION 3.10)
project(HeightOMatic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Modbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(MODBUS REQUIRED libmodbus)

# SDL2
find_package(SDL2 REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Sources
set(EXCAVATOR_SOURCES
    src/excavator/excavator.cpp
)

set(UI_SOURCES
    src/ui/ui.cpp
    src/ui/fonts.cpp
    src/ui/main_screen.cpp
    src/ui/calibrate_screen.cpp
    src/ui/sensor_setup_screen.cpp
    src/ui/sensor_raw_screen.cpp
)

add_executable(heightmatic
    src/main.cpp
    ${EXCAVATOR_SOURCES}
    ${UI_SOURCES}
)

target_include_directories(heightmatic PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${MODBUS_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(heightmatic PRIVATE
    ${MODBUS_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    pthread
    atomic
)
