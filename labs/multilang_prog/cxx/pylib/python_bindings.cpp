#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/stl.h>
#include "cxxlib/lib/multithreaded_vectorized_aligned_optimized/multithreaded_vectorized_aligned_optimized.hpp"
#include "cxxlib/lib/singlethreaded_vectorized_optimized/singlethreaded_vectorized_optimized.hpp"

namespace py = pybind11;

PYBIND11_MODULE(fft_pylib, m) {
    py::class_<Multithreaded_Vectorized_Aligned_Optimized::FFTConverter>(m, "FFTConverterParallel")
        .def_static("convert_parallel", &Multithreaded_Vectorized_Aligned_Optimized::FFTConverter::convert,
                    py::return_value_policy::copy,
                    "Convert a complex vector using parallel FFT");

    py::class_<SingleThreaded_Vectorized_Optimized::FFTConverter>(m, "FFTConverterSimple")
        .def_static("convert_simple", &SingleThreaded_Vectorized_Optimized::FFTConverter::convert,
                    py::return_value_policy::copy,
                    "Convert a complex vector using simple FFT");
}
