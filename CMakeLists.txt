cmake_minimum_required(VERSION 3.0)

project(ndm_test_2)

add_executable(ndm_test_2 main.cpp)
find_library(json11 "libjson11.so")
target_include_directories(ndm_test_2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_sources(ndm_test_2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/genlserver.cpp")
target_link_libraries(ndm_test_2 json11)
install(TARGETS ndm_test_2 RUNTIME DESTINATION bin)
