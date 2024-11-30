#include <singlethreaded_nonvectorized.hpp>
#include <cmath>
#include <numbers> 


namespace SingleThreaded_NonVectorized {

ComplexVector FFTConverter::convert(const ComplexVector& inputVector)
{
    auto result = inputVector;

    convertMutable(result);

    return result;
}

void FFTConverter::convertMutable(ComplexVector& inputVector)
{
    int n = inputVector.size();
    if (n <= 1) {
        return;
    }

    ComplexVector a0(n / 2), a1(n / 2);
    for (int i = 0; i < n / 2; i++) {
        a0[i] = inputVector[2*i];
        a1[i] = inputVector[2*i+1];
    }
    convertMutable(a0);
    convertMutable(a1);

    double ang = 2 * std::numbers::pi / n;
    ComplexValue w(1), wn(cos(ang), sin(ang));
    for (int i = 0; 2 * i < n; i++) {
        inputVector[i] = a0[i] + w * a1[i];
        inputVector[i + n/2] = a0[i] - w * a1[i];
        w *= wn;
    }
}

}
