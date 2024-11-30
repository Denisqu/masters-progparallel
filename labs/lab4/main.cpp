
#ifndef __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR
#define __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR
#endif

#include "libs/Simple-FFT/include/simple_fft/fft_settings.h"
#include "libs/Simple-FFT/benchmark-tests/benchmark_tests_fftw3.h"
#include "libs/Simple-FFT/unit-tests/test_fft.hpp"
#include <vector>
#include <complex>
#include <ctime>
#include <iostream>
#include <iomanip>


namespace simple_fft {
namespace fft_test {
    bool benchmark_simple_1D_fft()
    {   
        bool res = false;
        const char * err_str = NULL;
        using namespace pulse_params;
        // typedefing vectors
        typedef std::vector<real_type> RealArray1D;
        typedef std::vector<complex_type> ComplexArray1D;
        typedef std::vector<std::vector<real_type> > RealArray2D;
        typedef std::vector<std::vector<complex_type> > ComplexArray2D;
        typedef std::vector<std::vector<std::vector<real_type> > > RealArray3D;
        typedef std::vector<std::vector<std::vector<complex_type> > > ComplexArray3D;

        std::vector<real_type> t, x, y;
        makeGridsForPulse3D(t, x, y);

        // 1D fields and spectrum
        RealArray1D E1_real(nt);
        ComplexArray1D E1_complex(nt), G1(nt);
        const int numFFTLoops1D = 10000;

        // 2D fields and spectrum
        RealArray2D E2_real(nt);
        ComplexArray2D E2_complex(nt), G2(nt);

        // 3D fields and spectrum
        RealArray3D E3_real(nt);
        ComplexArray3D E3_complex(nt), G3(nt);

        int grid_size_t = static_cast<int>(nt);
        for(int i = 0; i < grid_size_t; ++i) {
            E2_real[i].resize(nx);
            E2_complex[i].resize(nx);
            G2[i].resize(nx);
        }
        int grid_size_x = static_cast<int>(nx);
        for(int i = 0; i < grid_size_t; ++i) {
            E3_real[i].resize(nx);
            E3_complex[i].resize(nx);
            G3[i].resize(nx);
            for(int j = 0; j < grid_size_x; ++j) {
                E3_real[i][j].resize(ny);
                E3_complex[i][j].resize(ny);
                G3[i][j].resize(ny);
            }
        }

        CMakeInitialPulses3D<RealArray1D,RealArray2D,RealArray3D,true>::makeInitialPulses(E1_real, E2_real, E3_real);
        CMakeInitialPulses3D<ComplexArray1D,ComplexArray2D,ComplexArray3D,false>::makeInitialPulses(E1_complex, E2_complex, E3_complex);

        // Measure the execution time of Simple FFT
        // 1) 1D Simple FFT for real data
        clock_t beginTime = clock();
        for(int i = 0; i < numFFTLoops1D; ++i) {
            res = FFT(E1_real, G1, nt, err_str);
            if (!res) {
                std::cout << "Simple FFT 1D real failed: " << err_str << std::endl;
                return res;
            }
        }
        //std::cout << "Simple 1D FFT for real data: execution time for "
        //          << numFFTLoops1D << " loops: " << std::setprecision(20)
        //          << real_type(clock() - beginTime)/CLOCKS_PER_SEC << std::endl;
    
        return res;
    }

}}



int main()
{
    auto result = bool { false }; 
    for (int i = 0; i < 50; ++i) {
        result = simple_fft::fft_test::benchmark_simple_1D_fft();
    }
    return result;
}