cmake_minimum_required(VERSION 3.2.0)
project(IFJ15 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pedantic -Wall -Werror")

add_executable(ifj15 main.c)

enable_testing()
find_library(cgreen cgreen)

add_library(hash_test hash_test.c hash.c)
add_library(ial_test ial_test.c ial.c)
add_library(interpreter_test interpreter_test.c interpreter.c)
add_library(expression_test expression_test.c expression.c)

add_executable(run_test run_test.c)
target_link_libraries(run_test cgreen hash_test ial_test interpreter_test expression_test)

add_test(hash run_test Hash)
add_test(ial run_test Ial)
add_test(interpreter run_test Interpreter)
add_test(interpreter run_test Expresion)
