cmake_minimum_required(VERSION 3.15)
project(sim VERSION "1.1.3")

configure_file("src/version.h.in" "version.h" @ONLY)
include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

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
    "src/scripting.h"
    "src/scripting.cpp"
    "src/simulation.h"
    "src/simulation.cpp"
    "src/util.h"
    "src/util.cpp")

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "third_party" "third_party/luajit/src")
target_link_libraries(${PROJECT_NAME} raylib)

if(MSVC)
    target_link_directories(${PROJECT_NAME} PUBLIC "bin")
    target_link_libraries(${PROJECT_NAME} lua51)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bin/lua51.dll
                ${PROJECT_BINARY_DIR}
        COMMENT "Copying lua51.dll to output directory...")

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND powershell.exe ${CMAKE_CURRENT_SOURCE_DIR}/release.ps1 ${PROJECT_VERSION}
                    $<TARGET_FILE:sim>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Packing release...")
    endif()
else()
    target_link_libraries(${PROJECT_NAME} luajit-5.1)
endif()
