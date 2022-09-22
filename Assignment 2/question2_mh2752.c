/*

CS-550 (Fall-2021)

Assignment 2 : Question 2

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
int team1_total_sequence_generated = 0;
int team2_total_sequence_generated = 0;

int team1_correct_sequence_generated = 0;
int team2_correct_sequence_generated = 0;

int team1_index_to_write_to = 0;
int team2_index_to_write_to = 0;

int winner_flag = -1;


// Global mutex variables:
pthread_mutex_t team1_mutex;
pthread_mutex_t team2_mutex;
pthread_mutex_t common_winner_mutex;


// The integer buffers:
int team1_buffer[num_of_threads];
int team2_buffer[num_of_threads];


// Argument struct:
typedef struct argument_object
{
	unsigned int thread_id;
} arg_obj;



// Function prototype for the do_work function:
void *do_work_team_1(void *args);
void *do_work_team_2(void *args);


int main()
{
	
	// A variable for indicating
	// array indices:
	int index;

	// Thread objects arrays:
	pthread_t team1_worker_threads[num_of_threads];
	pthread_t team2_worker_threads[num_of_threads];

	// Argument objects pointer arrays:
	arg_obj *team1_args_pointer_array[num_of_threads];
	arg_obj *team2_args_pointer_array[num_of_threads];

	// Initializing the mutex objects:
	pthread_mutex_init(&team1_mutex,NULL);
	pthread_mutex_init(&team2_mutex,NULL);
	pthread_mutex_init(&common_winner_mutex,NULL);

	

	// Creating and initializing the
	// argument objects:
	
	// For team_1:
	for(index = 0; index < num_of_threads; index++)
	{
		// Create an argument object and store
		// a pointer to it in the array:
		team1_args_pointer_array[index] = (arg_obj*)calloc(1,sizeof(struct argument_object));
		team1_args_pointer_array[index]->thread_id = (index+1);

	}

	//For team_2:
	for(index = 0; index < num_of_threads; index++)
	{
		// Create an argument object and store
		// a pointer to it in the array:
		team2_args_pointer_array[index] = (arg_obj*)calloc(1,sizeof(struct argument_object));
		team2_args_pointer_array[index]->thread_id = (index+4);

	}





	// Creating the worker threads:
	
	int return_code;
	
	// For team_1:
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_create(&team1_worker_threads[index],NULL,do_work_team_1,(void *)team1_args_pointer_array[index]);

		// Check return code:
		if(return_code) // Return code is not zero. Error.
		{
			fprintf(stderr,"\nError in creating Team-1 Thread-%d",(index+1));
			exit(1);
		}

	}

	// For team_2:
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_create(&team2_worker_threads[index],NULL,do_work_team_2,(void *)team2_args_pointer_array[index]);

		// Check return code:
		if(return_code) // Return code is not zero. Error.
		{
			fprintf(stderr,"\nError in creating Team-1 Thread-%d",(index+1));
			exit(1);
		}

	}


	// Joining child threads:

	// For team_1:
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_join(team1_worker_threads[index],NULL);

		if(return_code) // Return code is not zero. Error!
		{
			fprintf(stderr,"\nError while joining Team-1 Thread-%d",(index+1));
			exit(1);
		}
	}

	// For team_2:
	for(index = 0; index < num_of_threads; index++)
	{
		return_code = pthread_join(team2_worker_threads[index],NULL);

		if(return_code) // Return code is not zero. Error!
		{
			fprintf(stderr,"\nError while joining Team-1 Thread-%d",(index+1));
			exit(1);
		}
	}




	// Printing out the total sequence generated and
	// number of correct sequences generated:
	fprintf(stdout,"\nTotal sequences generated team1: %d",team1_total_sequence_generated);
	fprintf(stdout,"\nNumber of correct sequences team1: %d",team1_correct_sequence_generated);

	fprintf(stdout,"\nTotal sequences generated team2: %d",team2_total_sequence_generated);
	fprintf(stdout,"\nNumber of correct sequences team2: %d",team2_correct_sequence_generated);




	// Freeing dynamically allocated memories:
	
	// For team_1:
	for(index = 0; index < num_of_threads; index++)
	{
		free(team1_args_pointer_array[index]);
	}

	// For team_2:
	for(index = 0; index < num_of_threads; index++)
	{
		free(team2_args_pointer_array[index]);
	}




	return 0;

}



// Defintiion of the do_work_team_1() function:
void *do_work_team_1(void *args)
{

	// Unpacking the arguments:
	arg_obj *arg_obj_pointer;
	arg_obj_pointer = (arg_obj*) args;
	int id = arg_obj_pointer->thread_id;


	// Auxiliary variable(s):
	int flag_123;
	bool competition_over;


	while(true)
	{
		// Acquire lock:
		pthread_mutex_lock(&team1_mutex);

		// If all correct sequences have been generated,
		// no need to go further. Unlock the mutex and
		// return to main thread:
		if(team1_correct_sequence_generated == 10)
		{
			pthread_mutex_unlock(&team1_mutex); // Releasing lock
			break; // Breaking out of the while loop
		}


		// All correct sequences have not been generated.
		// Go ahead with sequence generation.
		else
		{
			// You still have the lock. Go ahead.


			// Check if the competition is
			// over ot not:

			// Acquire lock on the winner flag:
			pthread_mutex_lock(&common_winner_mutex);

			competition_over = (winner_flag != -1);

			if(competition_over == true)
			{
				// Release the flag lock
				pthread_mutex_unlock(&common_winner_mutex);

				// Release the team1 lock
				pthread_mutex_unlock(&team1_mutex);

				// Return to main thread
				return NULL;
			}

			// Competition not over yet. Release
			// flag lock. COntinue with the next
			// steps.
			pthread_mutex_unlock(&common_winner_mutex);






			
			// Writing the integer to buffer
			team1_buffer[team1_index_to_write_to] = id; 
			
			// Printing thread id (1/2/3):
			fprintf(stdout,"\nMy id: %d",id);

			// Moving index to the next slot
			team1_index_to_write_to += 1; 


			// Check if the buffer is full.
			// In that case, index should be
			// equal to 3.
			if(team1_index_to_write_to == 3) // buffer full
			{
				// Incrementing total sequence number
				// count:
				team1_total_sequence_generated += 1;

				// Check if the sequence in the buffer is 123 or not:
				flag_123 = (team1_buffer[0] == 1) && (team1_buffer[1] == 2) && (team1_buffer[2] == 3);


				// Reset the buffer:
				team1_index_to_write_to = 0;


				// If sequence is 123, increment the count:
				if(flag_123 == true)
				{
					// Printing 123 on stdout:
					fprintf(stdout,"\n123");

					// Increment the count:
					team1_correct_sequence_generated += 1;

					// If all correct sequences have been generated,
					// release the lock and join main thread:
					if(team1_correct_sequence_generated == 10)
					{
						
						// Acquire lock on the winner flag:
						pthread_mutex_lock(&common_winner_mutex);

						// Check if the flag is still uncaptured:
						if(winner_flag == -1)
						{
							// Capture the flag:
							winner_flag = 1; // Team id of team 1

							// Print victory message:
							fprintf(stdout,"\nTeam 1 won!");
						}
						
						

						// Release the lock on the winner flag:
						pthread_mutex_unlock(&common_winner_mutex);



						// Release team 1 lock:
						pthread_mutex_unlock(&team1_mutex);

						// Return/join main:
						return NULL;
					}
					
				}

			}

			// Release lock:
			pthread_mutex_unlock(&team1_mutex);

			// Sleep for 500000 micro-seconds:
			usleep(500000);
		}
		
	} // While looop ends here.


	// Return to main thread:
	return NULL;



}








// Defintiion of the do_work_team_2() function:
void *do_work_team_2(void *args)
{

	// Unpacking the arguments:
	arg_obj *arg_obj_pointer;
	arg_obj_pointer = (arg_obj*) args;
	int id = arg_obj_pointer->thread_id;


	// Auxiliary variable(s):
	int flag_456;
	bool competition_over;


	while(true)
	{
		// Acquire lock:
		pthread_mutex_lock(&team2_mutex);

		// If all correct sequences have been generated,
		// no need to go further. Unlock the mutex and
		// return to main thread:
		if(team2_correct_sequence_generated == 10)
		{
			pthread_mutex_unlock(&team2_mutex); // Releasing lock
			break; // Breaking out of the while loop
		}


		// All correct sequences have not been generated.
		// Go ahead with sequence generation.
		else
		{
			// You still have the lock. Go ahead.

			// Acquire lock on the winner flag:
			pthread_mutex_lock(&common_winner_mutex);

			competition_over = (winner_flag != -1);

			if(competition_over == true)
			{
				// Release the flag lock
				pthread_mutex_unlock(&common_winner_mutex);

				// Release the team1 lock
				pthread_mutex_unlock(&team2_mutex);

				// Return to main thread
				return NULL;
			}

			// Competition not over yet. Release
			// flag lock. COntinue with the next
			// steps.
			pthread_mutex_unlock(&common_winner_mutex);


			
			// Writing the integer to buffer
			team2_buffer[team2_index_to_write_to] = id; 
			
			// Printing thread id (4/5/6):
			fprintf(stdout,"\nMy id: %d",id);

			// Moving index to the next slot
			team2_index_to_write_to += 1; 


			// Check if the buffer is full.
			// In that case, index should be
			// equal to 3.
			if(team2_index_to_write_to == 3) // buffer full
			{
				// Incrementing total sequence number
				// count:
				team2_total_sequence_generated += 1;

				// Check if the sequence in the buffer is 456 or not:
				flag_456 = (team2_buffer[0] == 4) && (team2_buffer[1] == 5) && (team2_buffer[2] == 6);


				// Reset the buffer:
				team2_index_to_write_to = 0;


				// If sequence is 456, increment the count:
				if(flag_456 == true)
				{
					// Printing 123 on stdout:
					fprintf(stdout,"\n456");

					// Increment the count:
					team2_correct_sequence_generated += 1;

					// If all correct sequences have been generated,
					// release the lock and join main thread:
					if(team2_correct_sequence_generated == 10)
					{
						
						// Acquire lock on the winner flag:
						pthread_mutex_lock(&common_winner_mutex);

						// Check if the flag is still uncaptured:
						if(winner_flag == -1)
						{
							// Capture the flag:
							winner_flag = 2; // Team id of team 2

							// Print victory message:
							fprintf(stdout,"\nTeam 2 won!");
						}


						// Release the lock on the winner flag:
						pthread_mutex_unlock(&common_winner_mutex);



						// Release team 1 lock:
						pthread_mutex_unlock(&team2_mutex);

						// Return/join main:
						return NULL;
					}
					
				}

			}

			// Release lock:
			pthread_mutex_unlock(&team2_mutex);

			// Sleep for 500000 micro-seconds:
			usleep(500000);
		}
		
	} // While looop ends here.


	// Return to main thread:
	return NULL;



}
