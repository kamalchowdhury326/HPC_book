/****************************************************************************** 
* Matrix-matrix multiplication using OpenMP with 2-D data distribution.       *
* This version uses a single parallel region and performs the mapping of      *
* threads to data blocks manually.                                            *
*                                                                             *
* Program illustrate the use of dynamic memory allocation to create           *
* contiguous 2D-matrices and use traditional array indexing.                  *
* It also illustrate the use of gettime to measure wall clock time.           *
*                                                                             *
* To Compile:                                                                 *
* gcc -Wall -O -fopenmp matmul_2d_parallel_regionV1.2.c  -o matmul_2d_parallel_regionV1.2             * 
* To run: ./matmul_2d_parallel_regionV1.2 <size> <P> <Q> <iterations>                                             *
*                                                                             *
*  Author: Purushotham Bangalore                                              *
*  Email: puri@uab.edu                                                        *
*  Date: January 9, 2016                                                      *
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include<string.h>

double gettime(void) {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

void file_write(char* path, double **A, int m, int n,int P,int Q,int iterations){
   // Specify the full path where you want to create the file
    //const char *path = "/scratch/ualmkc001/filename.txt";
    const char* name = path;
    const char* extension = ".txt";
    char size[10];
    snprintf(size, 10, "%d", n);
    char iter[10];
    snprintf(iter, 10, "%d", iterations);
    char pq_char[10];
    snprintf(pq_char, 10, "%d", P*Q);
    

    char* name_with_extension;
    name_with_extension = malloc(strlen(name)+1+10); /* make space for the new string (should check the return value ...) */
    strcpy(name_with_extension, name); /* copy name into the new var */
    
    strcat(name_with_extension,".");
    strcat(name_with_extension,size);
    strcat(name_with_extension,".");
    strcat(name_with_extension,iter);
  
    strcat(name_with_extension,".");
    strcat(name_with_extension,pq_char);

    strcat(name_with_extension, extension); /* add the extension */
    
    
    // Open file in write mode
    FILE *file = fopen(name_with_extension, "w");

    // Check if file creation is successful
    if (file == NULL) {
        printf("Failed to create the file.\n");
        return ;
    }
   
    for(int i=0;i<m;i++){
       for(int j=0;j<n;j++){
          // Write something to the file
         fprintf(file, " %lf ",A[i][j]);
       }
       fprintf(file, "\n");
    }
    

    // Close the file
    fclose(file);
    printf("File created successfully at %s\n", path);
    free(name_with_extension);
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
void matmul2(double **a, double **b, double ***c, int N, int P, int Q,int iterations) 
{
    int i, j, k;
    double sum;
    /* You could use: double **out = *c; 
       and replace (*c) below with out, 
       if you like to make referencing easier to understand */
    int step;
    #pragma omp parallel default(none) shared(a,b,c,N,P,Q,iterations) private(i,j,k,sum,step) num_threads(P*Q)
    {
    for(step=0;step<iterations;step++){

        int tid = omp_get_thread_num();
        int p = tid / Q;
        int q = tid % Q;
        int myM = N / P;
        int istart = p * myM;
        int iend = istart + myM;
        if (p == P-1) iend = N;
      #ifdef DEBUG0
        printf("tid=%d istart=%d iend=%d\n",tid,istart,iend);
      #endif
        for (i=istart; i<iend; i++) {
          int myN = N / Q;
          int jstart = q * myN;
          int jend = jstart + myN;
          if (q == Q-1) jend = N;
    #ifdef DEBUG0
        printf("tid=%d[p,q]=[%d,%d]: {istart,iend}:{%d,%d} {jstart,jend}:{%d,%d}\n", tid, p, q, istart, iend, jstart, jend);
    #endif

        for (j=jstart; j<jend; j++) {
          sum = 0.0;
          for (k=0; k<N; k++)
              sum += a[i][k]*b[k][j];
          (*c)[i][j] = sum;
        }
      }

      #pragma omp barrier 
      //a=(*c);
      #pragma omp for
      for(int ii=0;ii<N;ii++){
          for(int jj=0;jj<N;jj++){
            a[ii][jj]=(*c)[ii][jj];
          }
        }

    }

    
    }
}

int main(int argc, char **argv) 
{
    int N, P, Q,iterations;
    double **a=NULL, **b=NULL, **c=NULL;
    double starttime, endtime;

    if (argc != 5) {
      printf("Usage: %s <N> <P> <Q><iterations>\n", argv[0]);
      exit(-1);
    }
    
    N = atoi(argv[1]);
    P = atoi(argv[2]);
    Q = atoi(argv[3]);
    iterations=atoi(argv[4]);

    
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
    matmul2(a,b,&c,N,P,Q,iterations);
    endtime = gettime();

#ifdef DEBUG_PRINT
    printarray(a, N, N);
    printf("\n");
    printarray(b, N, N);
    printf("\n");
    printarray(c, N, N);
#endif
   
   
    file_write("/scratch/ualmkc001/C",a,N,N,P,Q,iterations);
    printf("Time taken for size %d = %lf seconds\n", N, endtime-starttime);

    freearray(a);
    freearray(b);
    freearray(c);

    return 0;
}
