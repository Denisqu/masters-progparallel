#pragma once

#include "defs.hpp"

namespace Multithreaded_Vectorized_Aligned_Optimized {


template <typename T>
struct AlignedAllocator {
    using value_type = T;

    AlignedAllocator() = default;

    template <typename U>
    constexpr AlignedAllocator(const AlignedAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n > std::size_t(-1) / sizeof(T))
            throw std::bad_alloc();
        if (auto p = static_cast<T*>(std::aligned_alloc(16, n * sizeof(T))))
            return p;
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::free(p);
    }
};

template <typename T, typename U>
bool operator==(const AlignedAllocator<T>&, const AlignedAllocator<U>&) { return true; }
template <typename T, typename U>
bool operator!=(const AlignedAllocator<T>&, const AlignedAllocator<U>&) { return false; }

using AlignedComplexVector = std::vector<std::complex<double>, AlignedAllocator<std::complex<double>>>;

class FFTConverter {

public:
    
    static AlignedComplexVector convert(const ComplexVector& inputVector);

private:

    static void convertMutable(AlignedComplexVector& inputVector);

};

} // namespace Multithreaded_Vectorized_Aligned_Optimized