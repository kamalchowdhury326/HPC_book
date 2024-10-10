/* File:     omp_private.c
 *
 * Purpose:  Print the value of a private variable at the beginning
 *           of a parallel block and after the end of the block
 *
 * Compile:  gcc -g -Wall -fopenmp -o omp_privateV1 omp_privateV1.c
 * Run:      ./omp_privateV1 <number of threads>
 * 
 * Input:    none
 * Output:   Value of int at various points in the program
 *
 * IPP:      Section 5.5.4 (p. 231)
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>   

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int x = 5;
   int thread_count = strtol(argv[1], NULL, 10); 

for(int i=0;i<2;i++){
#  pragma omp parallel num_threads(thread_count) \
      private(x)
   { 
      int my_rank = omp_get_thread_num();
      printf("Thread %d address=%llu > before initialization, x = %d\n", 
            my_rank,&my_rank, x);
      x = 2*my_rank + 2;
      printf("Thread %d address=%llu > after initialization, x = %d\n", 
            my_rank,&my_rank, x);
   }
}
   printf("After parallel block, x = %d\n", x);

   return 0; 
}  /* main */
