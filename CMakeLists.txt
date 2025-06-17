cmake_minimum_required(VERSION 3.10)
project(dmitree LANGUAGES C)

# Specify the C standard (optional)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add the executable target
add_executable(dmitree
    dmitree_win.c
)
