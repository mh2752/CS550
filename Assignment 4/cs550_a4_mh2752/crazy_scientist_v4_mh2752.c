/*

	CS-550 (Fall-2021)
	
	Assignment-4 (Problem-4)
	
	N.B.: Use the shell script provided for
	running this solution.
	
	Author:
		Nazmul (mh2752)
		
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <pthread.h>

#define SIZE 50
#define NUM_THREADS 2

void *do_crazy_computation(void* arg);

// The matrix:
double mat[SIZE][SIZE];



typedef struct Thread_Arguments
{  
  
  // Start and End Timestamps:
  double *start;
  double *end;
  
  //Lock object:
  pthread_mutex_t *row_lock;
  
  // Pointer to row index variable:
  int *row_index;
  
  // Pointer to iterations counter:
  int *iteration_count; 

  // Matrix pointer:
  double **matrix;

} T_Args;



int main(int argc, char **argv) 
{
   
  
  // Utility variables:
  int index,return_code;
  
  // Creating the thread arrays:
  pthread_t worker_threads[NUM_THREADS];
  
  // Creating the thread argument array:
  T_Args *thread_arguments[NUM_THREADS];
  
  // Declare and initialize lock:
  pthread_mutex_t lock;
  pthread_mutex_init(&lock,NULL); 
  
  // Indicating the indice of the row:
  int row = 0;
  
  // Timestamp variables:
  double start_0,start_1,end_0,end_1,start_main,end_main;
  
  // Iteration counters:
  int iteration_0,iteration_1;  
  
  start_main = omp_get_wtime();

  // Initialize thread arguments:
  for(index = 0; index < NUM_THREADS; index++)
  {
      if(index == 0)
      {
          thread_arguments[index] = (T_Args*)calloc(1,sizeof(T_Args));
                
          thread_arguments[index]->start = &start_0;
          thread_arguments[index]->end = &end_0;
          thread_arguments[index]->row_lock = &lock;
          thread_arguments[index]->row_index = &row;
          thread_arguments[index]->iteration_count = &iteration_0;
         
      
      }
      
      else
      {
          thread_arguments[index] = (T_Args*)calloc(1,sizeof(T_Args));  
              
          thread_arguments[index]->start = &start_1;
          thread_arguments[index]->end = &end_1;
          thread_arguments[index]->row_lock = &lock;
          thread_arguments[index]->row_index = &row;
          thread_arguments[index]->iteration_count = &iteration_1;
             
      
      }   
    
  
  }
  
  
  // Creating and initializing threads:
  for(index = 0; index < NUM_THREADS; index++)
  {
      return_code = pthread_create(&worker_threads[index],NULL,do_crazy_computation,(void*)thread_arguments[index]);
      
      if(return_code!=0)
      {
          fprintf(stderr,"Error in creating Thread-%d",index);
          fflush(stderr);
          exit(1);
      }    
  
  
  
  }
  
  
  // Joining threads:
  for(index = 0; index < NUM_THREADS; index++)
  {
    return_code = pthread_join(worker_threads[index],NULL);
    
    if(return_code!=0)
    {
        fprintf(stderr,"\n\tError in joining Thrtead-%d",index);
        fflush(stderr);
        exit(1);    
    }
  
  
  }
  
  end_main = omp_get_wtime();
  
  
  // Printing outputs:
  fprintf(stdout,"\n\tNumber of iterations for Thread-0: %d\n",iteration_0);
  fflush(stdout);
  
  fprintf(stdout,"\tNumber of iterations for Thread-1: %d\n",iteration_1);
  fflush(stdout);
  
  fprintf(stdout,"\tTotal execution time (sanity check): %f\n",(end_main-start_main));
  fflush(stdout); 
  
    
  fprintf(stdout,"\tTime Thread1: %f\n",(end_0 - start_0));
  fflush(stdout);
  
  fprintf(stdout,"\tTime Thread2: %f\n",(end_1 - start_1));
  fflush(stdout);
  
  fprintf(stdout,"\tLoad imbalance: %f\n\n",fabs((end_0 - start_0) - (end_1 - start_1)));
  fflush(stdout);
  
  
  // De-allocate T_Args memories:
  for(index = 0; index < NUM_THREADS; index++)
  {  
      free(thread_arguments[index]);  
  }  
  

  exit(0);
}

//Crazy computation
void *do_crazy_computation(void *arg) 
{

  // Unpack the arguments:
  T_Args *arg_ptr = (T_Args*)arg;
  
  double *start_timestamp = arg_ptr->start;
  double *end_timestamp = arg_ptr->end;
  
  pthread_mutex_t *lock = arg_ptr->row_lock;
  
  
  int *row = arg_ptr->row_index;
  int *iterations = arg_ptr->iteration_count;  
  
  // Utility variable:
  int i,j;
  
  
  // Get starting timestamp:
  *(start_timestamp) = omp_get_wtime();
  
  
  while(1)
  {
      
      // Acquire lock:
      pthread_mutex_lock(lock);
      
      // If 50 rows have been processed,
      // return to main:
      if(*(row)==50)
      {   
          // Get ending timestamp:
          *(end_timestamp) = omp_get_wtime();
          // Release lock:
          pthread_mutex_unlock(lock);
          return NULL; 
      
      }
      
      // Not all rows have been processed. Go ahead
      // with executing this iteration:
      else
      {
        i = *(row); // The current row that has to be processed
        
        *(row) = *(row)+1; // Next row that should be processed
        
        // Release lock:
        pthread_mutex_unlock(lock);
        
        int iter;
        double value;
        
        // Do the calculation and update the matrix:
        for(j = 0; j < SIZE; j++)
        {   
             
             value = 0.0;
             
             for (iter = 0; iter < 5*i*i*i+1 + j*j*j+1; iter++)
             {
                 value +=  (cos(i*value) + sin(j*value));
             }
             
             mat[i][j] = value;
            
        
        }        
        // Update the iteration count:
        *(iterations) = *(iterations)+1;       
      
      }
  
  
  }


}

