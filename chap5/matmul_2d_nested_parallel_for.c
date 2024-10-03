/****************************************************************************** 
*  Matrix-matrix multiplication using OpenMP with 2-D data distribution.      *
*  This version uses nested parallel for loops.                               *
*                                                                             *
*  Program illustrate the use of dynamic memory allocation to create          *
*  contiguous 2D-matrices and use traditional array indexing.                 *
*  It also illustrate the use of gettime to measure wall clock time.          *
*                                                                             *
*  To Compile: gcc -Wall -O -fopenmp matmul_2d_nested_parallel_for.c          * 
*              add -DPTHREADS=P -DQTHREADS=Q to create PXQ threads            *
*  To run:                                                                    *
*          export OMP_NESTED=TRUE                                             *
*          ./a.out <size> <P> <Q>                                             *
*                                                                             *
*  Author: Purushotham Bangalore                                              *
*  Email: puri@uab.edu                                                        *
*  Date: January 9, 2016                                                      *
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef PTHREADS
#define PTHREADS 1
#endif
#ifndef QTHREADS
#define QTHREADS 1
#endif

double gettime(void) {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

double **allocarray(int P, int Q) {
  int i;
  double *p, **a;
  
  p = (double *)malloc(P*Q*sizeof(double));
  a = (double **)malloc(P*sizeof(double*));

  if (p == NULL || a == NULL) 
    printf("Error allocating memory\n");

  /* for row major storage */
  for (i = 0; i < P; i++)
    a[i] = &p[i*Q];
  
  return a;
}

void freearray(double **a) {
  free(&a[0][0]);
  free(a);
}

double **initarray(double **a, int mrows, int ncols, double value) {
  int i,j;

  for (i=0; i<mrows; i++)
    for (j=0; j<ncols; j++)
      // a[i][j] = drand48()*value;
      a[i][j] = value;
  
  return a;
}

void printarray(double **a, int mrows, int ncols) {
  int i,j;
  
  for (i=0; i<mrows; i++) {
    for (j=0; j<ncols; j++)
      printf("%f ", a[i][j]);
    printf("\n");
  }
}

/* return the output array address as return value */
double **matmul1(double **a, double **b, double **c, int N) 
{
    int i, j, k;
    double sum;

    for (i=0; i<N; i++)
      for (j=0; j<N; j++) {
        sum = 0.0;
	for (k=0; k<N; k++)
	  sum += a[i][k]*b[k][j];
        c[i][j] = sum;
      }

    return c;
}

/* output array address is passed as an argument */
void matmul2(double **a, double **b, double ***c, int N) 
{
    int i, j, k;
    double sum;
    /* You could use: double **out = *c; 
       and replace (*c) below with out, 
       if you like to make referencing easier to understand */
       
#pragma omp parallel for default(none) shared(a,b,c,N) private(i) num_threads(PTHREADS)
    for (i=0; i<N; i++)
    #pragma omp parallel for default(none) shared(a,b,c,N,i) private(j,k,sum) num_threads(QTHREADS)
      for (j=0; j<N; j++) {
        sum = 0.0;
	for (k=0; k<N; k++)
	  sum += a[i][k]*b[k][j];
	(*c)[i][j] = sum;
      }
}

int main(int argc, char **argv) 
{
    int N;
    double **a=NULL, **b=NULL, **c=NULL;
    double starttime, endtime;

    if (argc != 2) {
      printf("Usage: %s <N>\n", argv[0]);
      exit(-1);
    }
    
    N = atoi(argv[1]);
    
    /* Allocate memory for all three matrices and temporary arrays */
    a = allocarray(N, N);
    b = allocarray(N, N);
    c = allocarray(N, N);
    
    /* Initialize the matrices */
    srand48(123456);
    a = initarray(a, N, N, (double)(1.0));
    b = initarray(b, N, N, (double)(2.0));
    c = initarray(c, N, N, (double)0.0);

    /* Perform matrix multiplication */
    starttime = gettime();
    // c = matmul1(a,b,c,N);
    matmul2(a,b,&c,N);
    endtime = gettime();

#ifdef DEBUG_PRINT
    printarray(a, N, N);
    printf("\n");
    printarray(b, N, N);
    printf("\n");
    printarray(c, N, N);
#endif

    printf("Time taken for size %d = %lf seconds\n", N, endtime-starttime);

    freearray(a);
    freearray(b);
    freearray(c);

    return 0;
}
