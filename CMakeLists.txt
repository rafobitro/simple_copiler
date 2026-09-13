cmake_minimum_required(VERSION 3.10)
project(YuCompiler C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(compiler
    src/compiler.c
    src/lexer.c
    src/parser.c
    src/codegen.c
    src/text_buffer.c
)
