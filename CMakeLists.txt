cmake_minimum_required(VERSION 3.10)
project(BankingProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

add_subdirectory(banking)
add_subdirectory(tests)
