
#include <iostream>
#include <vector>
#include <thread>
#include <omp.h>

namespace {

bool is_prime(std::uint64_t num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (std::uint64_t i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

std::uint64_t sum_of_primes(const std::vector<std::uint64_t>& numbers) {
    std::uint64_t sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < numbers.size(); ++i) {
        if (is_prime(numbers[i])) {
            sum += numbers[i];
        }
    }

    return sum;
}

}

int main()
{
    const auto kNumbersSize = 10000000;
    auto numbers = std::vector<std::uint64_t >(kNumbersSize);
    for (size_t i = 0; i < numbers.size(); ++i) {
        numbers[i] = i;
    }

#ifdef _OPENMP
    std::cout << "using openmp..." << std::endl;
    const auto num_threads = std::thread::hardware_concurrency();
    omp_set_num_threads(num_threads > 4 ? num_threads : 4);
#endif

    const auto result = sum_of_primes(numbers);
    
    std::cout << "sum_of_primes with N = " << kNumbersSize 
              << " equals "                << result
              << std::endl;

    return 0;
}