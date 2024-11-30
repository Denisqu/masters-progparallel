#include "multithreaded_vectorized_aligned_optimized.hpp"

#include <cmath>
#include <numbers>
#include <tuple>
#include <cstring>
#include <omp.h>
#include <thread>
#include <iostream>

#include "add-on/complex/complexvec1.h"
#include <immintrin.h>
#include <vectorclass.h>


namespace {

using namespace Multithreaded_Vectorized_Aligned_Optimized;

double * pReal(std::complex<double>& value)
{
    return &reinterpret_cast<double *>( &value )[0];
}

double * pImage(std::complex<double>& value)
{
    return &reinterpret_cast<double *>( &value )[1];
}

void openmp_init()
{
    int max_threads = std::thread::hardware_concurrency();
    omp_set_num_threads(max_threads);
    std::cout << omp_get_num_threads();
}

size_t reverseBits(size_t x, int n) {
    size_t result = 0;
    for (int i = 0; i < n; i++, x >>= 1) {
        result = (result << 1) | (x & 1U);
    }
    return result;
}

void transformRadix2(AlignedComplexVector &inputVector) {
    size_t n = inputVector.size();
    int    levels = 0;
    for (size_t temp = n; temp > 1U; temp >>= 1) {
        levels++;
    }

    std::vector<Complex1d> expTable(n / 2);
    size_t             i;
    #pragma omp parallel for private(i) shared(expTable,n)
    for (i = 0; i < n / 2; i++) {
        auto value = std::polar(1.0, -2 * M_PI * i / n);
        expTable[i] = Complex1d();
        expTable[i].load(pReal(value));
    }

    #pragma omp parallel for private(i) shared(inputVector,n)
    for (i = 0; i < n; i++) {
        size_t j = reverseBits(i, levels);
        if (j > i) {
            std::swap(inputVector[i], inputVector[j]);
        }
    }

    size_t halfsize, size, j, k, tablestep;
    // use SSE2
    Complex1d  temp_vec;
    #pragma omp parallel private(size,halfsize,tablestep,i,j,k, temp_vec) shared(inputVector)
    {
    for (size = 2; size <= n; size *= 2) {
        halfsize  = size / 2;
        tablestep = n / size;
        #pragma omp for
        for (i = 0; i < n; i += size) {
            for (j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
                Complex1d temp_vec = [&]() -> Complex1d {
                    auto vec = Complex1d();
                    vec.load(pReal(inputVector[j + halfsize]));
                    vec *= expTable[k];
                    return vec;
                }();
                {
                    auto vec = Complex1d();
                    vec.load(pReal(inputVector[j]));
                    vec -= temp_vec;
                    vec.store(pReal(inputVector[j + halfsize]));
                }
                {
                    auto vec = Complex1d();
                    vec.load(pReal(inputVector[j]));
                    vec += temp_vec;
                    vec.store(pReal(inputVector[j]));
                }
            }
        }
        if (__builtin_expect(size == n, 0)) {
            break;
        }
    }
}
}

}

namespace Multithreaded_Vectorized_Aligned_Optimized {

AlignedComplexVector FFTConverter::convert(const ComplexVector& inputVector)
{
    openmp_init();
    AlignedComplexVector result(inputVector.size(), {0, 0});
    std::memcpy(result.data(), inputVector.data(), inputVector.size() * sizeof(ComplexVector::value_type));
    convertMutable(result);
    return result;
}

void FFTConverter::convertMutable(AlignedComplexVector& inputVector)
{
    int n = inputVector.size();
    if (__builtin_expect(n <= 1, 0)) {
        return;
    }

    transformRadix2(inputVector);
}

} // namespace Multithreaded_Vectorized_Aligned_Optimized

/* * * * * * * * * * * * * * * * * * * * * *
 *
 *      Работает медленнее:
        __m128d o = _mm_load_pd( (double *)&inputVector[i + n/2 ] );   // odd
        double cc = cos(-2.*std::numbers::pi*i / n);
        double ss = sin(-2.*std::numbers::pi*i / n);
        __m128d wr = _mm_load1_pd(&cc);			//__m128d wr =  _mm_set_pd( cc,cc );		// cc
        __m128d wi = _mm_set_pd(ss, -ss);		// -d | d	, note that it is reverse order
        // compute the w*o
        wr = _mm_mul_pd(o, wr);					// ac|bc
        __m128d n1 = _mm_shuffle_pd(o, o, _MM_SHUFFLE2(0, 1)); // invert
        wi = _mm_mul_pd(n1, wi);				// -bd|ad
        n1 = _mm_add_pd(wr, wi);				// ac-bd|bc+ad
        o = _mm_load_pd((double *)&inputVector[i]);		// load even part
        wr = _mm_add_pd(o, n1);					// compute even part, inputVector_e + w * inputVector_o;
        wi = _mm_sub_pd(o, n1);					// compute odd part,  inputVector_e - w * inputVector_o;
        _mm_store_pd((double *)&inputVector[i],wr);
        _mm_store_pd((double *)&inputVector[i + n/2],wi);
*/
