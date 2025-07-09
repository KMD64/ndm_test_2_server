cmake_minimum_required(VERSION 3.0)

project(ndm_test_2)

add_executable(ndm_test_2 main.cpp)

install(TARGETS ndm_test_2 RUNTIME DESTINATION bin)
