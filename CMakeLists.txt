cmake_minimum_required(VERSION 3.16)
project(pipesort C)

option(PIPESORT_BUILD_SHARED "Build shared library" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(pipesort_obj OBJECT
    src/psort_u.c
    src/psort_u128.c
    src/psort_u256.c
    src/psort_u512.c
    internal/pipe_sort_u128.c
    internal/pipe_sort_u256_idx_radix8.c
    internal/pipe_sort_u512_idx_radix8.c
)
add_executable(bench_u128_qsort tests/bench_u128_qsort.c)
target_link_libraries(bench_u128_qsort PRIVATE pipesort)
add_executable(test_all tests/test_all.c)
target_link_libraries(test_all PRIVATE pipesort)

target_include_directories(pipesort_obj PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

if(PIPESORT_BUILD_SHARED)
  add_library(pipesort SHARED $<TARGET_OBJECTS:pipesort_obj>)
else()
  add_library(pipesort STATIC $<TARGET_OBJECTS:pipesort_obj>)
endif()

set_target_properties(pipesort PROPERTIES OUTPUT_NAME "pipesort")

install(TARGETS pipesort
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/pipesort/ DESTINATION include/pipesort FILES_MATCHING PATTERN "*.h")

