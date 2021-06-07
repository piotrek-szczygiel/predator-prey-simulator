cmake_minimum_required(VERSION 3.15)
project(sim VERSION "1.1.3")

configure_file("src/version.h.in" "version.h" @ONLY)
include_directories(${PROJECT_BINARY_DIR})

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(MINGW)
    set(CMAKE_CXX_STANDARD_LIBRARIES
        "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive"
    )
endif()

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
    "src/agent.h"
    "src/config.h"
    "src/config.cpp"
    "src/grid.h"
    "src/gui.h"
    "src/gui.cpp"
    "src/main.cpp"
    "src/map.h"
    "src/map.cpp"
    "src/pathfinder.cpp"
    "src/pathfinder.h"
    "src/platform.h"
    "src/platform.cpp"
    "src/simulation.h"
    "src/simulation.cpp"
    "src/util.h"
    "src/util.cpp")

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "third_party")
target_link_libraries(${PROJECT_NAME} raylib)
