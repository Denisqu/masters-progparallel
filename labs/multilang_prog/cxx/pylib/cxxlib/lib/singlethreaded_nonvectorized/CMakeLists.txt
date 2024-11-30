file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_library(singlethreaded_nonvectorized SHARED ${SOURCES} ${HEADERS} ${SHARED_HEADERS})
target_link_libraries(singlethreaded_nonvectorized PUBLIC sharedlib)
target_include_directories(singlethreaded_nonvectorized PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SHARED_HEADERS_PATH})