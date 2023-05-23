cmake_minimum_required(VERSION 3.16)
SET(CMAKE_CXX_STANDARD 20)

project(splatter)
include(FetchContent)

set(BUILD_SHARED_LIBS FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE STRING "don't update every time")
set(CPPWINRT_REQUIRED_COMPILER_FLAGS "/std:c++latest /await")

FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/lsalzman/enet
)

FetchContent_MakeAvailable(enet)

FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.24.1
)

FetchContent_MakeAvailable(SDL)

#glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG master
)

FetchContent_MakeAvailable(glm)

include_directories("${PROJECT_BINARY_DIR}/_deps/enet-src/include")
include_directories("${PROJECT_SOURCE_DIR}")

#add_subdirectory(relay)
add_subdirectory(game)
