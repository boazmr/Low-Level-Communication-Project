cmake_minimum_required(VERSION 3.23)
project(chat)

file(GLOB MySources *.c source/*.c)
add_executable(MyProgram ${MySources})
