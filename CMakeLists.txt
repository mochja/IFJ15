cmake_minimum_required(VERSION 3.2.0)
project(IFJ15 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pedantic -Wall -Werror")

add_executable(ifj15 main.c)

enable_testing()
find_library(cgreen cgreen)

add_library(hash_test hash_test.c hash.c)

add_executable(run_test run_test.c)
target_link_libraries(run_test cgreen hash_test)

add_test(hash run_test hash)
