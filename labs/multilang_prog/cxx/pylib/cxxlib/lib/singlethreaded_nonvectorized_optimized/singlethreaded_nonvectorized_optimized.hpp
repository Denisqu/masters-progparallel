#pragma once

#include "defs.hpp"

namespace SingleThreaded_NonVectorized_Optimized {


class FFTConverter {

public:
    
    static ComplexVector convert(const ComplexVector& inputVector);

private:

    static void convertMutable(ComplexVector& inputVector);

};

} // namespace SingleThreaded_NonVectorized