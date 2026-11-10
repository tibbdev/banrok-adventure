cmake_minimum_required(VERSION 3.15)
project(banrok-adventure C CXX)

set(CMAKE_C_STANDARD   17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure Static Runtime for MSVC globally before targets are defined
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(USE_WAYLAND ON CACHE BOOL "" FORCE)     # Recommended Raylib wayland flag for Linux

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "6.2"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# Adding our source files
file(GLOB_RECURSE GAME_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/")

# Declaring our executable
add_executable(${PROJECT_NAME})

# Static C/C++ Runtime Configuration per Platform/Compiler
if(MSVC)
    # Managed via CMAKE_MSVC_RUNTIME_LIBRARY above
elseif(MINGW OR (UNIX AND NOT APPLE))
    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++
    )
elseif(APPLE)
    # Link native macOS frameworks required by Raylib
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

target_sources(${PROJECT_NAME} PRIVATE ${GAME_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable