enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(runTests test.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread
                      singlethreaded_nonvectorized
                      singlethreaded_nonvectorized_optimized
                      singlethreaded_vectorized_optimized
                      singlethreaded_vectorized_aligned_optimized
                      multithreaded_vectorized_aligned_optimized)
