project(game)

#SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.0.22
)

FetchContent_MakeAvailable(SDL)

#cglm

set(CGLM_SHARED FALSE)
set(CGLM_STATIC TRUE)
set(CGLM_USE_C99 FALSE) # C11 

FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm
    GIT_TAG master
)

FetchContent_MakeAvailable(cglm)

set(BUILD_SHARED_LIBS FALSE)

set(game_src_paths "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")


file(GLOB_RECURSE game_SRC ${game_src_paths})

set_source_files_properties(game_SRC PROPERTIES LANGUAGE CXX)


# link libraries

set(game_libs enet SDL2-static SDL2main glm)

if (WIN32)
list(APPEND game_libs ws2_32 winmm)
endif()

if(NOT WINDOWS_STORE)
    list(APPEND game_libs "SDL2main") # don't link SDL2main on xbox since it uses a c++ entrypoint
endif()


# don't show console in release build

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(splatter 
        ${game_SRC}
    )
else()
    add_executable(splatter WIN32
        ${game_SRC}
        ${APP_ICON_RESOURCE_WINDOWS}
            src/Texture.cpp src/Texture.h src/GameObject.cpp src/GameObject.h src/Player.cpp src/Player.h src/Bullet.cpp src/Bullet.h src/NetworkLayer.h src/Serialization.h)
endif()

target_link_libraries(splatter PRIVATE ${game_libs})
target_compile_definitions(splatter PRIVATE _CRT_SECURE_NO_WARNINGS=1)
target_include_directories(splatter PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_include_directories(splatter PUBLIC "${PROJECT_SOURCE_DIR}/src/lib")


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # In non-win32 debug build, debug_malloc is on by default
  target_compile_definitions(splatter PRIVATE DEBUG=1)
endif()

