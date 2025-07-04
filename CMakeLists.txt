cmake_minimum_required(VERSION 3.15)

project(poker)

set(BUILD_CARD5 OFF CACHE BOOL "" FORCE)
set(BUILD_CARD6 OFF CACHE BOOL "" FORCE)
set(BUILD_CARD7 ON CACHE BOOL "" FORCE)
set(BUILD_PLO4 OFF CACHE BOOL  "" FORCE)
set(BUILD_PLO5 OFF CACHE BOOL  "" FORCE)
set(BUILD_PLO6 OFF CACHE BOOL  "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/pokerhandevaluator/cpp)

add_subdirectory(src)
target_link_libraries(poker PRIVATE pheval)