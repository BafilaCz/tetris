cmake_minimum_required(VERSION 3.10)
project(tetris C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -fsanitize=address -Wno-unused-variable -Wuninitialized -Werror=vla")

include_directories(include)

set(SOURCES
    src/main.c
    src/tetris.c
    src/ui.c
    src/utils.c
    src/score.c
)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(tetris ${SOURCES})

# RUČNÍ LINKOVÁNÍ SDL2_ttf a SDL2_mixer
target_link_libraries(tetris
    ${SDL2_LIBRARIES}
    SDL2_ttf
    SDL2_mixer
    m
)
