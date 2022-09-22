/*

	CS-550 (Fall-2021)
	
	Assignment-4 (Problem-1)
	
	N.B.: Use the provided shell
	script to run this solution.
	
	Author:
	  Nazmul (mh2752)

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define SIZE 50
#define NUM_THREADS 2

double do_crazy_computation(int i,int j);


int main(int argc, char **argv) 
{
  double mat[SIZE][SIZE];
  int i,j;


  double tstart=omp_get_wtime();

  //modify code here

  // Setting number of threads:
  omp_set_num_threads(NUM_THREADS);

  
  // Multi-threading:
  #pragma omp parallel shared(mat) private(i,j)
  {

    int tid = omp_get_thread_num();
    
    #pragma omp sections
    {
      #pragma omp section
      {
        for (i=0;i<SIZE/2;i++) 
        { 
           for (j=0;j<SIZE;j++) 
           {  
              mat[i][j] = do_crazy_computation(i,j);
           }
        }  
    
      }
      
      #pragma omp section
      {
        for (i=SIZE/2;i<SIZE;i++) 
        { 
              for (j=0;j<SIZE;j++) 
              {  
                mat[i][j] = do_crazy_computation(i,j);              
              }
        }       
      }    
    
    }
   

  }  

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  printf("\n\tElapsed time: %f seconds\n\n",elapsed);

  exit(0);
}

//Crazy computation
double do_crazy_computation(int x,int y) 
{
   int iter;
   double value=0.0;

   for (iter = 0; iter < 5*x*x*x+1 + y*y*y+1; iter++) 
   {
     value +=  (cos(x*value) + sin(y*value));
   }
  return value;
}

