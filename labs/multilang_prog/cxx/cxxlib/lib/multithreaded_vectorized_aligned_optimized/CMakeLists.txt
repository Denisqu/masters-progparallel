file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

find_package(OpenMP REQUIRED)


add_library(multithreaded_vectorized_aligned_optimized SHARED ${SOURCES} ${HEADERS} ${SHARED_HEADERS})
target_link_libraries(multithreaded_vectorized_aligned_optimized PUBLIC sharedlib vector_class_lib OpenMP::OpenMP_CXX)
target_include_directories(multithreaded_vectorized_aligned_optimized PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SHARED_HEADERS_PATH})

