#!/bin/bash
# script to run the foo program on asax
source /apps/profiles/modules_asax.sh.dyn
module load intel
icx -g -Wall -o omp_mat_mat_mul_v2.3 omp_mat_mat_mul_v2.3.c -fopenmp


# ./omp_mat_mat_mul_v2.3 500 500 1
# ./omp_mat_mat_mul_v2.3 500 500 2
# ./omp_mat_mat_mul_v2.3 500 500 20

cd /scratch/ualmkc001/
diff C.500.500.1.txt C.500.500.2.txt
diff C.500.500.2.txt C.500.500.20.txt





# ./omp_mat_mat_mul_v2.3 5000 5000 2
# ./omp_mat_mat_mul_v2.3 5000 5000 4
# ./omp_mat_mat_mul_v2.3 5000 5000 8
# ./omp_mat_mat_mul_v2.3 5000 5000 10
# ./omp_mat_mat_mul_v2.3 5000 5000 16
# ./omp_mat_mat_mul_v2.3 5000 5000 20

# echo "gcc compiler"
# module load gcc/11.3.0
# gcc -g -Wall -o omp_mat_mat_mul_v2.3 omp_mat_mat_mul_v2.3.c -fopenmp -DDEBUG1
# ./omp_mat_mat_mul_v2.3 5000 5000 1
# ./omp_mat_mat_mul_v2.3 5000 5000 2
# ./omp_mat_mat_mul_v2.3 5000 5000 4
# ./omp_mat_mat_mul_v2.3 5000 5000 8
# ./omp_mat_mat_mul_v2.3 5000 5000 10
# ./omp_mat_mat_mul_v2.3 5000 5000 16
# ./omp_mat_mat_mul_v2.3 5000 5000 20

