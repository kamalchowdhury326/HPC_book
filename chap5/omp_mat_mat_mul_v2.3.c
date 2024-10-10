/* File:     
 *     omp_mat_mat_mul_v2.3.c 
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
 *     With DEBUG flag, A, B
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Compile:  
 *    gcc -g -Wall -o omp_mat_mat_mul_v2.3 omp_mat_mat_mul_v2.3.c -fopenmp
 * Usage:
 *    omp_mat_mat_mul_v2.3 <size> <iterations> <thread_count> 
 *
 * Notes:  
 *     1.  Storage for A, B, C is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide both 
 *         m and n.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, B, and C is logical:  all three are 
 *         globally shared.
 *     5.  DEBUG compile flag will prompt for input of A, B, and
 *         print C
 *     6.  Uses the OpenMP library function omp_get_wtime() to
 *         return the time elapsed since some point in the past
 *
 * IPP:    Section 5.9 (pp. 253 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<string.h>

/* Serial functions */
void Get_args(int argc, char* argv[], int* thread_count_p, 
      int* m_p, int* n_p, int* iterations);
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char* prompt, double x[], int n);
void Print_matrix(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);

/* Parallel function */
void Omp_mat_mat_mul(double A[], double x[], double y[],
      int m, int n, int thread_count,int iterations);
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
   #ifdef DEBUG1
   for(int i=0;i<m;i++){
       for(int j=0;j<n;j++){
          // Write something to the file
         printf(" %lf ",B[i*n+j]);
       }
       printf("\n");
    }
   #endif

}
void file_write(char* path, double A[], int m, int n,int P,int iterations){
   // Specify the full path where you want to create the file
    //const char *path = "/scratch/ualmkc001/filename.txt";
    const char* name = path;
    const char* extension = ".txt";
    char size[10];
    snprintf(size, 10, "%d", n);
    char iter[10];
    snprintf(iter, 10, "%d", iterations);
    char p_char[10];
    snprintf(p_char, 10, "%d", P);
    

    char* name_with_extension;
    name_with_extension = malloc(strlen(name)+1+10); /* make space for the new string (should check the return value ...) */
    strcpy(name_with_extension, name); /* copy name into the new var */
    
    strcat(name_with_extension,".");
    strcat(name_with_extension,size);
    strcat(name_with_extension,".");
    strcat(name_with_extension,iter);
  
    strcat(name_with_extension,".");
    strcat(name_with_extension,p_char);

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
         fprintf(file, " %lf ",A[i*n+j]);
       }
       fprintf(file, "\n");
    }
    

    // Close the file
    fclose(file);
    printf("File created successfully at %s\n", path);
    free(name_with_extension);
}
/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int     thread_count;
   int     m, n;
   int iterations;
   double* A;
   double* B;
   double* C;
   //double* y;

   Get_args(argc, argv, &thread_count, &m, &n,&iterations);

   A = malloc(m*n*sizeof(double));
   B = malloc(m*n*sizeof(double));
   C = malloc(m*n*sizeof(double));
   //B = malloc(m*n*sizeof(double));
   
 # ifdef DEBUG
      Read_matrix("Enter the matrix", A, m, n);
      Print_matrix("We read", A, m, n);
      Read_matrix("Enter the matrix", B,m, n);
      Print_matrix("We read", B,m, n);
 # else
      Gen_matrix(A, m, n);
      //file_write("/scratch/ualmkc001/A.txt",A,m,n);
      //file_read("/scratch/ualmkc001/A.txt",A,m,n);
      // Print_matrix("We generated", A, m, n); 
      Gen_matrix(B,m, n);
      //file_write("/scratch/ualmkc001/B.txt",B,m,n);
      //file_read("/scratch/ualmkc001/B.txt",B,m,n);
      // Print_vector("We generated", x, n); 
 # endif
   
   
    
    
    
    
   Omp_mat_mat_mul(A, B, C, m, n, thread_count,iterations);

#  ifdef DEBUG
      Print_matrix("The product is", C, m,n);
#  else
      // Print_vector("The product is", y, m); 
      file_write("/scratch/ualmkc001/C",C,m,n,thread_count,iterations);
      //file_write("/scratch/ualmkc001/C.txt",C,m,n);
      //file_read("/scratch/ualmkc001/C.txt",C,m,n);

#  endif

   free(A);
   free(B);
   free(C);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p, iterations
 */
void Get_args(int argc, char* argv[], int* thread_count_p, 
      int* m_p, int* n_p, int* iterations)  {

   if (argc != 4) Usage(argv[0]);
   *m_p = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[1], NULL, 10);
   *iterations=strtol(argv[2], NULL, 10);
   *thread_count_p = strtol(argv[3], NULL, 10);
   
   if (*thread_count_p <= 0 || *m_p <= 0 || *n_p <= 0) Usage(argv[0]);

}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <size> <iterations> <thread_count>\n", prog_name);
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
 * Function:  Omp_mat_mat_mul
 * Purpose:   Multiply an mxn matrix by an nx1 column vector
 * In args:   A, x, m, n, thread_count
 * Out arg:   y
 */
void Omp_mat_mat_mul(double A[], double B[], double C[],
      int m, int n, int thread_count, int iterations) {
   int i, j,k;
   double start, finish, elapsed;
   double x=0;
   int phase,tid;
   start = omp_get_wtime();
   #  pragma omp parallel num_threads(thread_count) default(none) \
   private(i,j,k,x,tid,phase)  shared(A, B, C, m, n,thread_count,iterations)
   for(phase=0; phase < iterations;phase++){
   
   {
      tid = omp_get_thread_num();
      int segment=m/thread_count;
      int i_start=tid*segment;
      int i_end=i_start+segment;
      int reminder=m%thread_count;
        if(tid==thread_count-1){
           i_end+=reminder;
        }
      # ifdef DEBUG2
      printf("phase=%d tid=%d i_start=%d i_end=%d\n",phase,tid,i_start,i_end);
      # endif
        //#pragma omp for
        for (i = i_start; i < i_end; i++) {
           
            for (j = 0; j < n; j++){
                x=0;
                
                for (k = 0; k < n; k++){
                        x += A[i*n+k]*B[k*n+j];
                        //printf("x=%lf i=%d j=%d k=%d\n",x,i,j,k);
                }
                    
                C[i*n+j]=x;
                #ifdef DEBUG2
                printf("phase=%d (i,j)=[%d,%d] tid=%d C[%d]=%lf \n",phase,i,j,tid,i*n+j,C[i*n+j]);
                #endif
                }
            
            
        }

   }
        
        
      #pragma omp barrier
         
         for(int ii=0;ii<n;ii++){
            for(int jj=0;jj<n;jj++){
                A[ii*n+jj]=C[ii*n+jj];
            }
        }

      
        
        
  }
   finish = omp_get_wtime();
   elapsed = finish - start;
   printf("Elapsed time for size=%d, iterations=%d, threads=%d, Takes %e seconds\n", n,iterations,thread_count,elapsed);

}  /* Omp_mat_mat_mul */


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
