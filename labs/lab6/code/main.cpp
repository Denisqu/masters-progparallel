#include <omp.h>
#include <stdio.h>
#include <thread>
#include <iostream>

int main(int argc, char** argv)
{

#ifdef _OPENMP
    std::cout << "using openmp..." << std::endl;
    const auto num_threads = std::thread::hardware_concurrency();
    omp_set_num_threads(num_threads > 4 ? num_threads : 4);
#endif

    #pragma omp parallel default(private)
    {
        const auto count=omp_get_thread_num();
        const auto ItsMe=omp_get_num_threads();
        #pragma omp critical
        {
            printf("Hello, OpenMP! I am %d of %d\n", count,ItsMe);
        }
    }
    return 0;
}