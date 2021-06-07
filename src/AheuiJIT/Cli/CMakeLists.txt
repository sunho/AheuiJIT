add_executable(AheuiJITCli main.cpp)
target_link_libraries(AheuiJITCli AheuiJIT)

install (TARGETS AheuiJITCli RUNTIME DESTINATION bin)