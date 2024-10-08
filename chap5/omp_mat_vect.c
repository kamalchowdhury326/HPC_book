/* File:     
 *     omp_mat_vect.c 
 *
 *
 * Purpose:  
 *     Computes a parallel matrix-vector product.  Matrix
 *     is distributed by block rows.  Vectors are distributed by 
 *     blocks.  Unless the DEBUG flag is turned on this version 
 *     uses a random number generator to generate A and x.
 *
 * Input:
 *     None unless compiled with DEBUG flag.
 *     With DEBUG flag, A, x
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Compile:  
 *    gcc -g -Wall -o omp_mat_vect omp_mat_vect.c -fopenmp
 * Usage:
 *    omp_mat_vect <thread_count> <m> <n>
 *
 * Notes:  
 *     1.  Storage for A, x, y is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide both 
 *         m and n.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, x, and y is logical:  all three are 
 *         globally shared.
 *     5.  DEBUG compile flag will prompt for input of A, x, and
 *         print y
 *     6.  Uses the OpenMP library function omp_get_wtime() to
 *         return the time elapsed since some point in the past
 *
 * IPP:    Section 5.9 (pp. 253 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Serial functions */
void Get_args(int argc, char* argv[], int* thread_count_p, 
      int* m_p, int* n_p);
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char* prompt, double x[], int n);
void Print_matrix(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);

/* Parallel function */
void Omp_mat_vect(double A[], double x[], double y[],
      int m, int n, int thread_count);
void file_read(char* path,double B[],int m, int n){
   // Specify the path to the input file
    //const char *path = "/scratch/ualmkc001/A.txt";
    
    // Open the file in read mode
    FILE *file1 = fopen(path, "r");

    // Check if the file opened successfully
    if (file1 == NULL) {
        printf("Failed to open the file.\n");
        return ;
    }

    
    for(int i=0;i<m;i++){
       for(int j=0;j<n;j++){
          // Write something to the file
         //fscanf(file, "%lf",&B[i*n+j]);
         if (fscanf(file1, "%lf", &B[i*n+j]) != 1) {
            printf("Error reading from file.\n");
            fclose(file1);
            return ;
         }
       }
       
    }
    // Close the file
    fclose(file1);
   #ifndef DEBUG1
   for(int i=0;i<m;i++){
       for(int j=0;j<n;j++){
          // Write something to the file
         printf(" %lf ",B[i*n+j]);
       }
       printf("\n");
    }
   #endif

}
void file_write(char* path, double A[], int m, int n){
   // Specify the full path where you want to create the file
    //const char *path = "/scratch/ualmkc001/filename.txt";
    
    // Open file in write mode
    FILE *file = fopen(path, "w");

    // Check if file creation is successful
    if (file == NULL) {
        printf("Failed to create the file.\n");
        return ;
    }
   
    for(int i=0;i<m;i++){
       for(int j=0;j<n;j++){
          // Write something to the file
         fprintf(file, " %lf ",A[i*n+j]);
       }
       fprintf(file, "\n");
    }
    

    // Close the file
    fclose(file);
    printf("File created successfully at %s\n", path);
}
/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int     thread_count;
   int     m, n;
   double* A;
   //double* B;
   double* x;
   double* y;

   Get_args(argc, argv, &thread_count, &m, &n);

   A = malloc(m*n*sizeof(double));
   x = malloc(n*sizeof(double));
   y = malloc(m*sizeof(double));
   //B = malloc(m*n*sizeof(double));
   
 # ifdef DEBUG
      Read_matrix("Enter the matrix", A, m, n);
      Print_matrix("We read", A, m, n);
      Read_vector("Enter the vector", x, n);
      Print_vector("We read", x, n);
 # else
      Gen_matrix(A, m, n);
      file_write("/scratch/ualmkc001/A.txt",A,m,n);
      file_read("/scratch/ualmkc001/A.txt",A,m,n);
      // Print_matrix("We generated", A, m, n); 
      Gen_vector(x, n);
      file_write("/scratch/ualmkc001/x.txt",x,n,1);
      file_read("/scratch/ualmkc001/x.txt",x,n,1);
      // Print_vector("We generated", x, n); 
 # endif
   
   
    
    
    
    
   Omp_mat_vect(A, x, y, m, n, thread_count);

#  ifdef DEBUG
      Print_vector("The product is", y, m);
#  else
      // Print_vector("The product is", y, m); 
      file_write("/scratch/ualmkc001/y.txt",y,m,1);
      file_read("/scratch/ualmkc001/y.txt",y,m,1);

#  endif

   free(A);
   free(x);
   free(y);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p
 */
void Get_args(int argc, char* argv[], int* thread_count_p, 
      int* m_p, int* n_p)  {

   if (argc != 4) Usage(argv[0]);
   *thread_count_p = strtol(argv[1], NULL, 10);
   *m_p = strtol(argv[2], NULL, 10);
   *n_p = strtol(argv[3], NULL, 10);
   if (*thread_count_p <= 0 || *m_p <= 0 || *n_p <= 0) Usage(argv[0]);

}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void Read_matrix(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
   # ifdef DEBUG1
   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = 1.0;
   #else
   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
   #endif
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
   # ifdef DEBUG1
   int i;
   for (i = 0; i < n; i++)
      x[i] = 2.0;
   #else
   int i;
   for (i = 0; i < n; i++)
      x[i] = random()/((double) RAND_MAX);
   #endif

}  /* Gen_vector */

/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}  /* Read_vector */


/*------------------------------------------------------------------
 * Function:  Omp_mat_vect
 * Purpose:   Multiply an mxn matrix by an nx1 column vector
 * In args:   A, x, m, n, thread_count
 * Out arg:   y
 */
void Omp_mat_vect(double A[], double x[], double y[],
      int m, int n, int thread_count) {
   int i, j;
   double start, finish, elapsed;

   start = omp_get_wtime();
#  pragma omp parallel for num_threads(thread_count)  \
      default(none) private(i, j)  shared(A, x, y, m, n)
   for (i = 0; i < m; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
         y[i] += A[i*n+j]*x[j];
   }
   finish = omp_get_wtime();
   elapsed = finish - start;
   printf("Elapsed time = %e seconds\n", elapsed);

}  /* Omp_mat_vect */


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix( char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%4.1f ", A[i*n + j]);
      printf("\n");
   }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char* title, double y[], double m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
}  /* Print_vector */
