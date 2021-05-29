cmake_minimum_required(VERSION 3.17)
project(sim)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib 3.0 QUIET)

if(NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)

    FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/master.tar.gz)

    FetchContent_GetProperties(raylib)
    if(NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)

        set(BUILD_EXAMPLES
            OFF
            CACHE BOOL "" FORCE) # don't build the supplied examples

        # build raylib
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})

    endif()
endif()

add_executable(
    ${PROJECT_NAME}
    "src/main.cpp"
    "src/util.h"
    "src/util.cpp"
    "src/simulation.h"
    "src/agent.h"
    "src/simulation.cpp"
    "src/platform.h"
    "src/platform.cpp"
    "src/simulation_values.h"
    "src/map.h")

target_link_libraries(${PROJECT_NAME} raylib)
