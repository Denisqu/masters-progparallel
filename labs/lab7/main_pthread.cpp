#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <memory>
#include <cmath>
#include <pthread.h>
#include <gmp.h>

namespace {

template <typename Func, typename... Args>
auto measure_execution_time(Func func, Args&&... args) 
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Function execution time: " << duration.count() * 1000 << " ms" << std::endl;
    return result;
}

struct ThreadData {
    int thread_id;
    int start; // начальный индекс, который будет обрабатывать этот поток
    int end;   // конечный индекс
    int a;     // a * x
    int b;     // b * y
    int n;     // степень бинома
    std::vector<std::string>* substrings;
};

void binomialCoefficient(mpz_t result, unsigned long long n, unsigned long long k)
{
    mpz_t n_mpz, k_mpz;
    mpz_init_set_ui(n_mpz, n);
    mpz_init_set_ui(k_mpz, k);

    mpz_bin_ui(result, n_mpz, k);

    mpz_clear(n_mpz);
    mpz_clear(k_mpz);
}

void* computeSubstrings(void* arg)
{
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int k = data->start; k < data->end; ++k) {
        mpz_t binomCoeff;
        mpz_init(binomCoeff);
        binomialCoefficient(binomCoeff, data->n, k);

        // Calculate (a^(n-k) * b^k) and multiply with binomial coefficient
        mpz_t term, temp, a_mpz, b_mpz;
        mpz_init(term);
        mpz_init(temp);
        mpz_init_set_ui(a_mpz, data->a);
        mpz_init_set_ui(b_mpz, data->b);

        // a^(n-k)
        mpz_pow_ui(term, a_mpz, data->n - k);

        // b^k
        mpz_pow_ui(temp, b_mpz, k);
        
        // Multiply (a^(n-k) * b^k)
        mpz_mul(term, term, temp);

        // Multiply by binomial coefficient
        mpz_mul(term, term, binomCoeff);

        char* term_str = mpz_get_str(nullptr, 10, term);
        std::string substring(term_str);
        if (k > 0) substring = " + " + substring;

        substring += "*x^" + std::to_string(data->n - k);
        if (k > 0) substring += "*y^" + std::to_string(k);

        (*data->substrings)[k] = substring;

        mpz_clear(binomCoeff);
        mpz_clear(term);
        mpz_clear(temp);
        mpz_clear(a_mpz);
        mpz_clear(b_mpz);
        free(term_str);
    }
    return nullptr;
}

std::string binomialExpansion(int a, int b, int x, int y, int n, int num_threads)
{
    std::string result = "(" + std::to_string(a) + " * x + " + std::to_string(b) + " * y)^" + std::to_string(n) + " = ";
    std::vector<std::string> substrings(n + 1);

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int chunk_size = (n + 1) / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].thread_id = i;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? (n + 1) : ((i + 1) * chunk_size);
        thread_data[i].a = a;
        thread_data[i].b = b;
        thread_data[i].n = n;
        thread_data[i].substrings = &substrings;

        pthread_create(&threads[i], nullptr, computeSubstrings, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
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
    unsigned int n = 20000;

    for (int i = 1; i < 19; ++i) {
        std::cout << "N_threads = " << i << std::endl;
        measure_execution_time(binomialExpansion, a, b, 1, 1, n, i);
    }
    return 0;
}
