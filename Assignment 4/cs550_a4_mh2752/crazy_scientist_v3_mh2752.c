/*

	CS-550 (Fall-2021)
	
	Assignment-4 (Problem-3)
	
	N.B.: Use the shell script provided for
	running this solution.
	
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
  
  int tid;

  // Setting number of threads:
  omp_set_num_threads(NUM_THREADS);
  
  // Variables for Thread 0 & 1's
  // timestamps:
  double start_0,start_1,end_0,end_1,total_0,total_1;

  
  // Multi-threading:
  #pragma omp parallel shared(mat) private(i,j,tid) 
  {    
    
    	tid = omp_get_thread_num();
    	
    	if(tid == 0)
    	{
    	      // Get starting timestamp for thread 0: 
	      start_0 = omp_get_wtime();
	}
	else
	{
		// Get starting timestamp for thread 1: 
	        start_1 = omp_get_wtime();
	}
	      
	      #pragma omp for nowait schedule(dynamic)         	
	      for (i=0;i<SIZE;i++) 
	      { 
		 for (j=0;j<SIZE;j++) 
		 {  
		    mat[i][j] = do_crazy_computation(i,j);
		 }
	      }       
	       
	 if(tid == 0)
	 {
	 	// Get ending timestamp for thread 0:
	 	end_0 = omp_get_wtime();
	 
	 }
	 else
	 {
	 	// Get ending timestamp for thread 1:
	 	end_1 = omp_get_wtime();	 
	 }      
    
    }  
  
   

  

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  
  // Thread 0 & 1's totals:
  total_0 = end_0 - start_0;
  total_1 = end_1 - start_1;
  
  // Calculating the load imbalance
  // between Thread 0 & 1:
  double load_imbalance = fabs((total_0 - total_1)); 
  
  // Output durations:
  printf("\n\tTotla Time (sanity check): %f seconds\n",elapsed);
  printf("\tTime Thread1: %f\n",total_0);
  printf("\tTime Thread2: %f\n",total_1);
  printf("\tLoad imbalance: %f\n\n",load_imbalance);

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

