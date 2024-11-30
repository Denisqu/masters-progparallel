#include <singlethreaded_nonvectorized_optimized.hpp>
#include <cmath>
#include <numbers> 
#include <iostream>

namespace SingleThreaded_NonVectorized_Optimized {

ComplexVector FFTConverter::convert(const ComplexVector& inputVector)
{
    auto result = inputVector;

    convertMutable(result);

    return result;
}

void FFTConverter::convertMutable(ComplexVector& inputVector)
{
    int n = inputVector.size();
    if (__builtin_expect(n <= 1, 0)) {
        return;
    }

    ComplexVector a0(n / 2), a1(n / 2);
    for (int i = 0; i < n / 2; ++i) {
        a0[i] = inputVector[2*i];
        a1[i] = inputVector[2*i+1];
    }
    convertMutable(a0);
    convertMutable(a1);

    double ang = 2 * std::numbers::pi / n;
    ComplexValue w(1), wn(cos(ang), sin(ang));
    for (int i = 0; 2 * i < n; ++i) {
        const auto value = w * a1[i];
        inputVector[i] = a0[i] + value;
        inputVector[i + n/2] = a0[i] - value;
        w *= wn;
    }
}



} // namespace SingleThreaded_NonVectorized_Optimized
