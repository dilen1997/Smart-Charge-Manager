cmake_minimum_required(VERSION 3.10)
project(EasyChargeSystem C)
cmake_minimum_required(VERSION 3.10)
project(EasyChargeSystem C)

set(CMAKE_C_STANDARD 99)

include_directories(scr)
include_directories(Input)

add_executable(EasyChargeSystem
    scr/main.c
    scr/station.c
    scr/cars.c
    scr/LoadFiles.c
    scr/display.c
    scr/port.c       
    scr/menu.c
)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
