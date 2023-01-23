add_executable(relay main.cpp)

target_link_libraries(relay enet "ws2_32" "winmm")