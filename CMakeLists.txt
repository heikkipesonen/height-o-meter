cmake_minimum_required(VERSION 3.10)
project(HeightOMatic LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths to your locally built libmodbus
set(MODBUS_INCLUDE_DIR ~/libmodbus/src)
set(MODBUS_LIBRARY ~/libmodbus/src/.libs/libmodbus.so)

add_executable(heightmatic main.cpp)   
target_link_libraries(heightmatic PRIVATE ${MODBUS_LIBRARY})
