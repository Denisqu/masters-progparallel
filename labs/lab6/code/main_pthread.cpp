#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <memory>
#include <cmath>
#include <pthread.h>
#include <gmp.h>

struct ThreadData {
    int thread_id;
    int start;
    int end;
    int a;
    int b;
    int n;
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

        mpz_t term, temp, a_mpz, b_mpz;
        mpz_init(term);
        mpz_init(temp);
        mpz_init_set_ui(a_mpz, data->a);
        mpz_init_set_ui(b_mpz, data->b);

        mpz_pow_ui(term, a_mpz, data->n - k);
        mpz_pow_ui(temp, b_mpz, k);
        mpz_mul(term, term, temp);
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

std::string binomialExpansion(int a, int b, int x, int y, int n)
{
    std::string result = "(" + std::to_string(a) + " * x + " + std::to_string(b) + " * y)^" + std::to_string(n) + " = ";
    std::vector<std::string> substrings(n + 1);

    const int num_threads = std::thread::hardware_concurrency();
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

int main()
{
    int a = 1, b = 2;
    unsigned int n = 10;

    const auto result = binomialExpansion(a, b, 1, 1, n);
    std::cout << "Result: " << result << std::endl;
    return 0;
}
