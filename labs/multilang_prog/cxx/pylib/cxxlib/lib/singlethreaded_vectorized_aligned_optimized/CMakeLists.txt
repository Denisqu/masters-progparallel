file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_library(singlethreaded_vectorized_aligned_optimized SHARED ${SOURCES} ${HEADERS} ${SHARED_HEADERS})
target_link_libraries(singlethreaded_vectorized_aligned_optimized PUBLIC sharedlib vector_class_lib)
target_include_directories(singlethreaded_vectorized_aligned_optimized PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SHARED_HEADERS_PATH})