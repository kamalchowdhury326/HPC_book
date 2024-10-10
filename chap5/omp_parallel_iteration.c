/* File:     
 *     omp_parallel_iterationV1.c 
 *
 *
 * Purpose:  
 *     Check the parallel for how it works.
 *
 *
 * Compile:  
 *    gcc omp_parallel_iterationV1.c -o omp_parallel_iterationV1 -fopenmp 
 * Usage:
 *    omp_parallel_iterationV1 <thread_count> <iterations> <size>
 *
 * 
 */
 #include "stdio.h"
#include <omp.h>
#include <stdlib.h>
void loop(int thread_count, int iterations, const int N) {
    #pragma omp parallel num_threads(thread_count)
    {
        int start_thread = omp_get_thread_num();
        printf("start thread %d\n", start_thread);
        for (int phase = 0; phase < iterations; phase++) {
            printf("\titeration %d, thread num %d\n", phase, omp_get_thread_num());
            #pragma omp for
            //#pragma omp parallel for
            for (int i = 0; i < N; i++) {
                printf("\t\t inner loop i=%d, thread num %d\n", i, omp_get_thread_num());
                //#pragma omp for
                for(int j=0;j<N;j++){
                    printf("\t\t inner inner loop (i,j)=[%d,%d], thread num %d\n",i, j, omp_get_thread_num());
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", argv[0]);
        exit(0);
    }
    int thread_count = strtol(argv[1], NULL, 10);
    int iterations = strtol(argv[2], NULL, 10);
    int N = strtol(argv[3], NULL, 10);
    
    loop(thread_count,iterations,N);
}