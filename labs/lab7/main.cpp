#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <cmath>
#include <omp.h>
#include <gmp.h> 
#include <thread>

namespace {

void binomialCoefficient(mpz_t result, unsigned long long n, unsigned long long k)
{
    // Convert n and k to GMP variables
    mpz_t n_mpz, k_mpz;
    mpz_init_set_ui(n_mpz, n);
    mpz_init_set_ui(k_mpz, k);

    mpz_bin_ui(result, n_mpz, k);  // GMP function for binomial coefficient C(n, k)

    // Clear GMP variables for n and k
    mpz_clear(n_mpz);
    mpz_clear(k_mpz);
}

std::string binomialExpansion(int a, int b, int x, int y, int n)
{
    std::string result = "(" + std::to_string(a) + " * x + " + std::to_string(b) + " * y)^" + std::to_string(n) + " = ";
    std::vector<std::string> substrings(n + 1);

    #pragma omp parallel for shared(substrings)
    for (int k = 0; k <= n; ++k) {
        mpz_t binomCoeff;
        mpz_init(binomCoeff); 
        binomialCoefficient(binomCoeff, n, k);

        // Calculate (a^(n-k) * b^k) and multiply with binomial coefficient
        mpz_t term;
        mpz_init(term);
        mpz_t a_mpz, b_mpz;
        mpz_init_set_ui(a_mpz, a);  // Convert a to GMP
        mpz_init_set_ui(b_mpz, b);  // Convert b to GMP

        // a^(n-k)
        mpz_pow_ui(term, a_mpz, n - k);

        // b^k
        mpz_t temp;
        mpz_init(temp);
        mpz_pow_ui(temp, b_mpz, k);

        // Multiply (a^(n-k) * b^k)
        mpz_mul(term, term, temp);

        // Multiply by binomial coefficient
        mpz_mul(term, term, binomCoeff);

        char* term_str = mpz_get_str(nullptr, 10, term);
        std::string substring(term_str);
        if (k > 0) substring = " + " + substring;

        substring += "*x^" + std::to_string(n - k);
        if (k > 0) substring += "*y^" + std::to_string(k);

        substrings[k] = substring;

        // Clear GMP variables
        mpz_clear(binomCoeff);
        mpz_clear(term);
        mpz_clear(temp);
        mpz_clear(a_mpz);
        mpz_clear(b_mpz);
    }

    for (const auto& substr : substrings) {
        result += substr;
    }

    return result;
}

}

int main()
{
    int a = 1, b = 2;
    unsigned int n = 10;

#ifdef _OPENMP
    std::cout << "using openmp..." << std::endl;
    const auto num_threads = std::thread::hardware_concurrency();
    omp_set_num_threads(n < num_threads ? n : num_threads);
#endif

    const auto result = binomialExpansion(a, b, 1, 1, n);
    std::cout << "Result: " << result << std::endl;
    return 0;
}


/*

n = 10000:

single Thread:
real    0m1.233s
user    0m1.168s
sys     0m0.065s

OpenMP:
real    0m0.292s
user    0m1.660s
sys     0m0.184s

PThreads:
TODO

*/