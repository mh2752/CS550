/*

CS-550 (Fall-2021)

Assignment 2 : Question 1

Author:
	Nazmul (mh2752@nau.edu)

*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// Number of worker threads:
#define num_of_threads  3 

//  Global Variables:
int total_sequence_generated = 0;
int correct_sequence_generated = 0;
int index_to_write_to = 0;

// Global mutex variable:
pthread_mutex_t mutex;


// The integer buffer:
int buffer[num_of_threads];


// Argument struct:
struct argument_object
{
	unsigned int thread_id;
};



// Function prototype for the do_work function:
void *do_work(void *args);


int main()
{
	
	// A variable for indicating
	// array indices:
	int index;

	// Thread objects array:
	pthread_t worker_threads[num_of_threads];

	// Argument objects pointer array:
	struct argument_object *args_pointer_array[num_of_threads];

	// Initializing the mutex object:
	pthread_mutex_init(&mutex,NULL);


	// Creating and initializing the
	// argument objects:
	for(index = 0; index < num_of_threads; index++)
	{
		// Create an argument object and store
		// a pointer to it in the array:
		args_pointer_array[index] = (struct argument_object*)calloc(1,sizeof(struct argument_object));
		args_pointer_array[index]->thread_id = (index+1);

	}


	// Creating the threads:
	int return_code;
	
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_create(&worker_threads[index],NULL,do_work,(void *)args_pointer_array[index]);

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
		return_code = pthread_join(worker_threads[index],NULL);

		if(return_code) // Return code is not zero. Error!
		{
			fprintf(stderr,"\nError while joining Thread-%d",(index+1));
			exit(1);
		}
	}


	// Printing out the total sequence generated and
	// number of correct sequences generated:
	fprintf(stdout,"\nTotal sequences generated: %d",total_sequence_generated);
	fprintf(stdout,"\nNumber of correct sequences: %d",correct_sequence_generated);


	// Freeing dynamically allocated memories:
	for(index = 0; index < num_of_threads; index++)
	{
		free(args_pointer_array[index]);
	}



	return 0;

}



// Defintiion of the do_work() function:
void *do_work(void *args)
{

	// Unpacking the arguments:
	struct argument_object *arg_obj_pointer;
	arg_obj_pointer = (struct argument_object*) args;
	int id = arg_obj_pointer->thread_id;


	// Auxiliary variable(s):
	int flag_123;


	while(true)
	{
		// Acquire lock:
		pthread_mutex_lock(&mutex);

		// If all correct sequences have been generated,
		// no need to go further. Unlock the mutex and
		// return to main thread:
		if(correct_sequence_generated == 10)
		{
			pthread_mutex_unlock(&mutex); // Releasing lock
			break; // Breaking out of the while loop
		}


		// All correct sequences have not been generated.
		// Go ahead with sequence generation.
		else
		{
			// You still have the lock. Go ahead.
			
			// Writing the integer to buffer
			buffer[index_to_write_to] = id; 
			
			// Printing thread id (1/2/3):
			fprintf(stdout,"\nMy id: %d",id);

			// Moving index to the next slot
			index_to_write_to += 1; 


			// Check if the buffer is full.
			// In that case, index should be
			// equal to 3.
			if(index_to_write_to == 3) // buffer full
			{
				// Incrementing total sequence number
				// count:
				total_sequence_generated += 1;

				// Check if the sequence in the buffer is 123 or not:
				flag_123 = (buffer[0] == 1) && (buffer[1] == 2) && (buffer[2] == 3);


				// Reset the buffer:
				index_to_write_to = 0;


				// If sequence is 123, increment the count:
				if(flag_123 == true)
				{
					// Printing 123 on stdout:
					fprintf(stdout,"\n123");

					// Increment the count:
					correct_sequence_generated += 1;

					// If all correct sequences have been generated,
					// release the lock and join main thread:
					if(correct_sequence_generated == 10)
					{
						// Release lock:
						pthread_mutex_unlock(&mutex);

						// Return/join main:
						return NULL;
					}
					
				}

			}

			// Release lock:
			pthread_mutex_unlock(&mutex);

			// Sleep for 500000 micro-seconds:
			usleep(500000);
		}
		
	} // While looop ends here.


	// Return to main thread:
	return NULL;



}
