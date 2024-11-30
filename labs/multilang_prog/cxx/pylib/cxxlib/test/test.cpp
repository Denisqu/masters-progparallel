#include <gtest/gtest.h>
#include "singlethreaded_nonvectorized.hpp"
#include "singlethreaded_nonvectorized_optimized.hpp"
#include "singlethreaded_vectorized_optimized.hpp"
#include "singlethreaded_vectorized_aligned_optimized.hpp"
#include "multithreaded_vectorized_aligned_optimized.hpp"

namespace {

const ComplexVector& sineWave(uint32_t numPoints = 0, double freq = 0) {
    static ComplexVector signal(numPoints);
    static bool isInit = false;
    auto i = 0;
    if (!isInit) {
        for (int i = 0; i < numPoints; i++) {
            signal[i] = std::complex<double>(sin(2 * std::numbers::pi * freq * i / numPoints), 0);
        }
        isInit = true;
    }
    return signal;
}

bool vectors_equal(const auto& vec1, const auto& vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }
    return std::equal(vec1.begin(), vec1.end(), vec2.begin());
}

} // namespace

TEST(FFT_OF_SINEWAVE_SINGLETHREADED_NON_VECTORIZED, FFT)
{
    //GTEST_SKIP() << "done";

    const auto result = SingleThreaded_NonVectorized::FFTConverter::convert(sineWave());

    ASSERT_EQ(result.empty(), false);
}


TEST(FFT_OF_SINEWAVE_SINGLETHREADED_NON_VECTORIZED_OPTIMIZED, FFT)
{
    //GTEST_SKIP() << "done";

    const auto result = SingleThreaded_NonVectorized_Optimized::FFTConverter::convert(sineWave());
    std::cout << "conversion ended" << std::endl;

    ASSERT_EQ(result.empty(), false);
}

TEST(FFT_OF_SINEWAVE_SINGLETHREADED_VECTORIZED_OPTIMIZED, FFT)
{
    //GTEST_SKIP() << "done";

    const auto result = SingleThreaded_Vectorized_Optimized::FFTConverter::convert(sineWave());

    std::cout << "conversion ended" << std::endl;

    ASSERT_EQ(result.empty(), false);
}

TEST(FFT_OF_SINEWAVE_SINGLETHREADED_VECTORIZED_ALIGNED_OPTIMIZED, FFT)
{
    //GTEST_SKIP() << "done";

    const auto result = SingleThreaded_Aligned_Vectorized_Optimized::FFTConverter::convert(sineWave());

    ASSERT_EQ(result.empty(), false);
}

TEST(FFT_OF_SINEWAVE_MULTITHREADED_VECTORIZED_ALIGNED_OPTIMIZED, FFT)
{
    //GTEST_SKIP() << "done";
    const auto result1 = Multithreaded_Vectorized_Aligned_Optimized::FFTConverter::convert(sineWave());

    ASSERT_EQ(result1.empty(), false);
}

int main(int argc, char** argv) {
    sineWave(1 << 26, 2.4e9);
    //sineWave(1 << 3, 5);

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    if (result != 0) {
        return result;
    }

    return 0;
}
