cmake_minimum_required(VERSION 3.2.0)
project(IFJ15 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pedantic -Wall -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

add_executable(ifj15 main.c scanner.c parser.c list.c token.h zval.h expr.c instruction.c vm.c ial.c)

enable_testing()
find_library(cgreen cgreen)

add_library(hash_test hash_test.c hash.c)
add_library(ial_test ial_test.c ial.c)
add_library(vm_test vm_test.c vm.c instruction.c)
add_library(expression_test expr_test.c expr.c)
add_library(instruction_test instruction_test.c instruction.c)

add_executable(run_test run_test.c)
target_link_libraries(run_test cgreen hash_test ial_test vm_test expression_test instruction_test)

add_test(hash run_test Hash)
add_test(ial run_test Ial)
add_test(VM run_test VM)
add_test(expression run_test Expression)
add_test(instruction run_test Instruction)
