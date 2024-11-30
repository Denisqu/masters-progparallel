cmake_minimum_required(VERSION 3.10)
project(fft_pylib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(cxxlib)

find_package(OpenMP REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

if (BUILD_PYTHON_LIB)
    pybind11_add_module(fft_pylib python_bindings.cpp)
    target_link_libraries(fft_pylib PRIVATE multithreaded_vectorized_aligned_optimized singlethreaded_vectorized_optimized sharedlib vector_class_lib OpenMP::OpenMP_CXX)
    install(TARGETS fft_pylib
            COMPONENT python
            LIBRARY DESTINATION ~/pythonlibs
    )
endif()
