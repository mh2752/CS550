/*

CS-550 (Fall-2021)

Assignment 2 : Question 3

Author:
	Nazmul (mh2752@nau.edu)

*/





#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// Number of worker threads:
#define num_of_threads  10 

//  Global Variables:
int total = 0;
int permitted_thread_id = 0;


// Global mutex variables:
pthread_mutex_t mutex;



// Argument struct:
typedef struct argument_object
{
	unsigned int thread_id;
} arg_obj;



// Function prototype for the do_work function:
void *do_work(void *args);



int main()
{
	
	// A variable for indicating
	// array indices:
	int index;

	// Thread objects arrays:
	pthread_t all_worker_threads[num_of_threads];
	
	// Argument objects pointer arrays:
	arg_obj *arg_obj_pointers[num_of_threads];	

	// Initializing the mutex objects:
	pthread_mutex_init(&mutex,NULL);	
	

	// Creating and initializing the
	// argument objects:
	for(index = 0; index < num_of_threads; index++)
	{
		// Create an argument object and store
		// a pointer to it in the array:
		arg_obj_pointers[index] = (arg_obj*)calloc(1,sizeof(arg_obj));
		arg_obj_pointers[index]->thread_id = index;	

	}

	// Creating the worker threads:	
	int return_code;
	
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_create(&all_worker_threads[index],NULL,do_work,(void *)arg_obj_pointers[index]);

		// Check return code:
		if(return_code) // Return code is not zero. Error.
		{
			fprintf(stderr,"\nError in creating Thread-%d",(index+1));
			exit(1);
		}
	}

	

	// Joining child threads:
	
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_join(all_worker_threads[index],NULL);

		if(return_code) // Return code is not zero. Error!
		{
			fprintf(stderr,"\nError while joining Thread-%d",(index+1));
			exit(1);
		}
	}

	fprintf(stdout,"\nTotal: %d",total);



	// Freeing dynamically allocated memories:	
	
	for(index = 0; index < num_of_threads; index++)
	{
		free(arg_obj_pointers[index]);
	}



	return 0;

}



// Defintiion of the do_work_team_1() function:
void *do_work(void *args)
{
	// Unpack the argument:
	arg_obj *arg_obj_pointer;
	arg_obj_pointer = (arg_obj*) args;
	int id = arg_obj_pointer->thread_id;


	// Auxiliary local variables:
	int loop_counter;
	int fprintf_return_value;



	// Execute the loop 22 times for each thread:
	for(loop_counter = 0; loop_counter < 22;)
	{
		// Acquire lock:
		pthread_mutex_lock(&mutex);

		// If in sequence, add:
		if(id == permitted_thread_id)
		{
			// Add to total:
			total = total + id;

			// Advance loop counter on
			// a successful add:
			loop_counter++;

			// Print to screen:
			fprintf_return_value = fprintf(stdout,"\nmy num: %d, total: %d",id,total);

			// If EOF received, output it to
			// stdout:
			if(fprintf_return_value == EOF)
			{
				fprintf(stderr,"\nCould not write message to stdout for Thread-%d and total: %d",id,total);
			}
			
			// Advance the permitted thread id
			// to allow for the next thread to
			// join:
			permitted_thread_id += 1;

			if(permitted_thread_id == 10) // Wrapping around
			{
				permitted_thread_id = 0;
			}

		}

		// Release lock:
		pthread_mutex_unlock(&mutex);
	}
	
	



	// Return to main thread:
	return NULL;
}





