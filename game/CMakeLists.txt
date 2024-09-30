project(game)


set(BUILD_SHARED_LIBS FALSE)

set(game_src_paths "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")


file(GLOB_RECURSE game_SRC ${game_src_paths})

set_source_files_properties(game_SRC PROPERTIES LANGUAGE CXX)


# link libraries

set(game_libs enet SDL3::SDL3 glm)

if (WIN32)
list(APPEND game_libs ws2_32 winmm)
endif()


# don't show console in release build

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(splatter 
        ${game_SRC}
    )
else()
    add_executable(splatter WIN32
        ${game_SRC}
        ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(splatter PRIVATE ${game_libs})
target_compile_definitions(splatter PRIVATE _CRT_SECURE_NO_WARNINGS=1)
target_include_directories(splatter PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_include_directories(splatter PUBLIC "${PROJECT_SOURCE_DIR}/src/lib")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # In non-win32 debug build, debug_malloc is on by default
  target_compile_definitions(splatter PRIVATE DEBUG=1)
endif()

