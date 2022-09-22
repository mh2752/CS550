/*

CS-550 (Fall-2021)

Assignment 2 : Question 4

Author:
	Nazmul (mh2752@nau.edu)

*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


//  Global Variable:
int counter = 0;



// Global mutex variable:
pthread_mutex_t mutex;


// Condition variables:
pthread_cond_t count_is_zero,count_is_ten;


// Function prototypes for the increment and decrement functions:
void *increment_counter(void *args);
void *decrement_counter(void *args);



int main()
{	
	// Utility variables:
	int index;

	// Thread objects:
	pthread_t incrementer_thread,decrementer_thread;	
	

	// Initializing the mutex objects:
	pthread_mutex_init(&mutex,NULL);

	// Initialize the condition variables:
	pthread_cond_init(&count_is_zero,NULL);
	pthread_cond_init(&count_is_ten,NULL);	
	

	// Creating the threads:	
	
	int return_code;
	
	// Incrementer thread:
	return_code = pthread_create(&incrementer_thread,NULL,increment_counter,NULL);
	// Check return code:
	if(return_code) // Return code is not zero. Error.
	{
		fprintf(stderr,"\nError in creating incrementer thread");
		exit(1);
	}

	// Decrementer thread:
	return_code = pthread_create(&decrementer_thread,NULL,decrement_counter,NULL);
	// Check return code:
	if(return_code) // Return code is not zero. Error.
	{
		fprintf(stderr,"\nError in creating decrementer thread");
		exit(1);
	}
	

	

	// Joining threads:
	
	return_code = pthread_join(incrementer_thread,NULL);

	if(return_code) // Return code is not zero. Error!
	{
		fprintf(stderr,"\nError while joining incrementer thread");
		exit(1);
	}


	return_code = pthread_join(decrementer_thread,NULL);

	if(return_code) // Return code is not zero. Error!
	{
		fprintf(stderr,"\nError while joining decrementer thread");
		exit(1);
	}
	
	

	
	fprintf(stdout,"\n");

	return 0;

}



// Counter increment:
void *increment_counter(void *args)
{

	// Local utility variable:
	int reps = 0;

	while(reps<3)
	{
	
		// Acquire lock:
		pthread_mutex_lock(&mutex);

		while(counter != 0)
		{
			//Wait for signal from decrementer:
			pthread_cond_wait(&count_is_zero,&mutex);
		}

		while(counter != 10)
		{
			counter++;
			fprintf(stdout,"\nCount is now (inc fn): %d",counter);
		}

		// Signal the decrementer thread:
		pthread_cond_signal(&count_is_ten);
		pthread_mutex_unlock(&mutex);

		reps++;

	}

	// Return to main thread:
	return NULL;
}



// Counter decrement:
void *decrement_counter(void *args)
{
	// Local utility variable:
	int reps = 0;

	while(reps<2)
	{
	
		// Acquire lock:
		pthread_mutex_lock(&mutex);

		while(counter != 10)
		{
			//Wait for signal from incrementer:
			pthread_cond_wait(&count_is_ten,&mutex);
		}

		while(counter != 0)
		{
			counter--;
			fprintf(stdout,"\nCount is now (dec fn): %d",counter);
		}

		// Signal the incrementer thread:
		pthread_cond_signal(&count_is_zero);
		pthread_mutex_unlock(&mutex);

		reps++;

	}

	// Return to main thread:
	return NULL;
}





