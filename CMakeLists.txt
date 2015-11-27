cmake_minimum_required(VERSION 3.2.0)
project(IFJ15 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pedantic -Wall -Werror")

add_executable(ifj15 main.c)

add_executable(hash_test hash_test.c hash.c)
target_link_libraries(hash_test cgreen)

enable_testing()
add_test(hash hash_test)