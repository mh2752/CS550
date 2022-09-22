/*
	CS-550 (Fall-2021)
	Assignment 3
	
	Author:
		Nazmul (mh2752@nau.edu)

*/



// ###################################### Necessary Imports ######################################

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

// ###############################################################################################



// ###################################### Macro & Misc. Definitions ##############################

#define NUM_ITERATIONS 10
#define NUM_LEFT_OVEN_MITTS 3
#define NUM_RIGHT_OVEN_MITTS 3



	// For representing type of baker:
	typedef enum Baker_Thread_Type {Lefty,Righty,Cautious} Baker_Thread_Type;

// ###############################################################################################



// #################################### User Function Prototype Declarations #####################

	void *do_work(void *arg);

// ################################################################################################


// ################################### Struct for Thread Arguments ################################

	typedef struct Thread_Arguments
	{
	
		// Type of baker this thread
		// represents:
		Baker_Thread_Type btt;
		
		// ID of the baker:
		int baker_id;		
		
		// Pointer to the lock object:
		pthread_mutex_t *left_lock;
		pthread_mutex_t *right_lock;
		
		// Pointers to condition variables:
		pthread_cond_t *left_handed_mitt_available;
		pthread_cond_t *right_handed_mitt_available;
		
		// Pointers to mitts counts:
		int *left_handed_mitts;
		int *right_handed_mitts;

		// Total wait time
		// of this thread:
		double thread_wait_time;	
	
	
	} T_Args;


// ################################################################################################



// #################################### Main Function #############################################

	int main(int argc, char **argv) 
	{

	  int num_left_handed_bakers;
	  int num_right_handed_bakers;
	  int num_cautious_bakers;
	  int seed;

	  /* 
	  	Process command-line arguments: 
	  */
	  if (argc != 5) 
	  {
	    fprintf(stderr,"Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n",argv[0]);
	    fflush(stderr);
	    exit(1);
	  }

	  if ((sscanf(argv[1],"%d",&num_left_handed_bakers) != 1) || 
	      (sscanf(argv[2],"%d",&num_right_handed_bakers) != 1) || 
	      (sscanf(argv[3],"%d",&num_cautious_bakers) != 1) || 
	      (sscanf(argv[4],"%d",&seed) != 1) ||
	      (num_left_handed_bakers < 1) ||
	      (num_right_handed_bakers < 1) ||
	      (num_cautious_bakers < 1) ||
	      (seed < 0)) {
	    fprintf(stderr,"Invalid command-line arguments... Aborting\n");
		fflush(stderr);
	    exit(1);
	  }
	 
	  /* Seed the RNG */
	  srand(seed);

	 
	 
	 // **************************************** Code Implementation *******************************************

	  			
		int index;
		int return_code;
		
		// Declaring the arrays containing thread pointers:
		pthread_t left_handed_bakers[num_left_handed_bakers];
		pthread_t right_handed_bakers[num_right_handed_bakers];
		pthread_t cautious_bakers[num_cautious_bakers];
		
		// Declaring the thread argument arrays:
		T_Args *args_for_lefty_threads[num_left_handed_bakers];
		T_Args *args_for_righty_threads[num_right_handed_bakers];
		T_Args *args_for_cautious_threads[num_cautious_bakers];
		
		
		// Declaring & initializing the lock objects
		pthread_mutex_t left_lock;
		pthread_mutex_init(&left_lock,NULL);
		
		pthread_mutex_t right_lock;
		pthread_mutex_init(&right_lock,NULL);
		
		// Declaring & initializing the condition variables:
		pthread_cond_t left_mitts_available;
		pthread_cond_init(&left_mitts_available,NULL);
		
		pthread_cond_t right_mitts_available;
		pthread_cond_init(&right_mitts_available,NULL);
		
		
		// Declaring & initializing available mitt counts;
		int available_left_mitts = NUM_LEFT_OVEN_MITTS; // From macro definitions
		int available_right_mitts = NUM_RIGHT_OVEN_MITTS; // From macro defintions		
		
		
		// Initializing thread argument arrays:
		
			// For lefty threads:
			for(index = 0; index < num_left_handed_bakers; index++)
			{
				args_for_lefty_threads[index] = (T_Args*)calloc(1,sizeof(T_Args));
				
				// Initializing with proper values:
				 args_for_lefty_threads[index]->btt = Lefty; // Type of baker
				 args_for_lefty_threads[index]->baker_id = index; // ID of the baker
				 args_for_lefty_threads[index]->left_lock = &left_lock; // The lock/mutex object
				 args_for_lefty_threads[index]->right_lock = &right_lock; // The lock/mutex object
				 args_for_lefty_threads[index]->left_handed_mitt_available = &left_mitts_available; // Condition variable
				 args_for_lefty_threads[index]->right_handed_mitt_available = &right_mitts_available; // Condition variable
				 args_for_lefty_threads[index]->left_handed_mitts = &available_left_mitts; // Available mitt count
				 args_for_lefty_threads[index]->right_handed_mitts = &available_right_mitts; // Available mitt count
			}		
			
			// For righty threads:
			for(index = 0; index < num_right_handed_bakers; index++)
			{
				args_for_righty_threads[index] = (T_Args*)calloc(1,sizeof(T_Args));
				
				// Initializing with proper values:
				 args_for_righty_threads[index]->btt = Righty; // Type of baker
				 args_for_righty_threads[index]->baker_id = index; // ID of the baker
				 args_for_righty_threads[index]->left_lock = &left_lock; // The lock/mutex object
				 args_for_righty_threads[index]->right_lock = &right_lock; // The lock/mutex object
				 args_for_righty_threads[index]->left_handed_mitt_available = &left_mitts_available; // Condition variable
				 args_for_righty_threads[index]->right_handed_mitt_available = &right_mitts_available; // Condition variable
				 args_for_righty_threads[index]->left_handed_mitts = &available_left_mitts; // Available mitt count
				 args_for_righty_threads[index]->right_handed_mitts = &available_right_mitts; // Available mitt count
			}
			
			
			// For cautious threads:
			for(index = 0; index < num_cautious_bakers; index++)
			{
				args_for_cautious_threads[index] = (T_Args*)calloc(1,sizeof(T_Args));
				
				// Initializing with proper values:
				 args_for_cautious_threads[index]->btt = Cautious; // Type of baker
				 args_for_cautious_threads[index]->baker_id = index; // ID of the baker
				 args_for_cautious_threads[index]->left_lock = &left_lock; // The lock/mutex object
				 args_for_cautious_threads[index]->right_lock = &right_lock; // The lock/mutex object
				 args_for_cautious_threads[index]->left_handed_mitt_available = &left_mitts_available; // Condition variable
				 args_for_cautious_threads[index]->right_handed_mitt_available = &right_mitts_available; // Condition variable
				 args_for_cautious_threads[index]->left_handed_mitts = &available_left_mitts; // Available mitt count
				 args_for_cautious_threads[index]->right_handed_mitts = &available_right_mitts; // Available mitt count
			}
		
		
			
		// Initializing thread arrays with created threads:
		
			// Lefty threads:
			for(index = 0; index < num_left_handed_bakers; index++)
			{
				return_code = pthread_create(&left_handed_bakers[index],NULL,do_work,(void *)args_for_lefty_threads[index]);
				
				if(return_code) // Return code is not zero. Error.
				{
					fprintf(stderr,"Error in creating Lefty Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}
			
			
			// Righty threads:
			for(index = 0; index < num_right_handed_bakers; index++)
			{
				return_code = pthread_create(&right_handed_bakers[index],NULL,do_work,(void *)args_for_righty_threads[index]);
				
				if(return_code) // Return code is not zero. Error.
				{
					fprintf(stderr,"\nError in creating Righty Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}
			
			// Cautious threads:
			for(index = 0; index < num_cautious_bakers; index++)
			{
				return_code = pthread_create(&cautious_bakers[index],NULL,do_work,(void *)args_for_cautious_threads[index]);
				
				if(return_code) // Return code is not zero. Error.
				{
					fprintf(stderr,"\nError in creating Cautious Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}

			
			
		// Joining all threads:
		
			// Lefty threads:
			for(index = 0; index < num_left_handed_bakers; index++)
			{
				return_code = pthread_join(left_handed_bakers[index],NULL);

				if(return_code) // Return code is not zero. Error!
				{
					fprintf(stderr,"Error while joining Lefty Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}
			
			// Righty threads:
			for(index = 0; index < num_right_handed_bakers; index++)
			{
				return_code = pthread_join(right_handed_bakers[index],NULL);

				if(return_code) // Return code is not zero. Error!
				{
					fprintf(stderr,"\nError while joining Right Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}
			
			// Cautious threads:
			for(index = 0; index < num_cautious_bakers; index++)
			{
				return_code = pthread_join(cautious_bakers[index],NULL);

				if(return_code) // Return code is not zero. Error!
				{
					fprintf(stderr,"Error while joining Cautious Thread-%d\n",index);
					fflush(stderr);
					exit(1);
				}
			}



		// Printing threads' wait times:

			// Lefties:
			for(index = 0; index < num_left_handed_bakers; index++)
			{
				fprintf(stderr,"[Left-handed baker %d] WAIT-TIME = %.5lf\n",index,args_for_lefty_threads[index]->thread_wait_time);
				fflush(stderr);
			}

			// Righties:
			for(index = 0; index < num_right_handed_bakers; index++)
			{
				fprintf(stderr,"[Right-handed baker %d] WAIT-TIME = %.3lf\n",index,args_for_righty_threads[index]->thread_wait_time);
				fflush(stderr);
			}

			// Cautious:
			for(index = 0; index < num_cautious_bakers; index++)
			{
				fprintf(stderr,"[Cautious baker %d] WAIT-TIME = %.3lf\n",index,args_for_cautious_threads[index]->thread_wait_time);
				fflush(stderr);
			}


		// Calculating average wait times:

		double sum_total = 0.0;

			// Lefties:
			for(index = 0; index < num_left_handed_bakers; index++)
			{
				sum_total += args_for_lefty_threads[index]->thread_wait_time;
			}
			fprintf(stderr,"Average wait time for left-handed threads is: %.3lf\n",(sum_total/(double)num_left_handed_bakers));
			sum_total = 0;
			fflush(stderr);

			// Righties:
			for(index = 0; index < num_right_handed_bakers; index++)
			{
				sum_total += args_for_righty_threads[index]->thread_wait_time;
			}
			fprintf(stderr,"Average wait time for right-handed threads is: %.3lf\n",(sum_total/(double)num_right_handed_bakers));
			sum_total = 0;
			fflush(stderr);

			// Cautious:
			for(index = 0; index < num_cautious_bakers; index++)
			{
				sum_total += args_for_cautious_threads[index]->thread_wait_time;
			}
			fprintf(stderr,"Average wait time for cautious threads is: %.3lf\n",(sum_total/(double)num_cautious_bakers));
			sum_total = 0;
			fflush(stderr);

			
		// De-allocating dynamically allocated memories:
		for(index = 0; index < num_left_handed_bakers; index++)
		{
			free(args_for_lefty_threads[index]);
		}
		for(index = 0; index < num_right_handed_bakers; index++)
		{
			free(args_for_righty_threads[index]);
		}
		for(index = 0; index < num_cautious_bakers; index++)
		{
			free(args_for_cautious_threads[index]);
		}

	// *********************************************************************************************************





	  exit(0);
	}

// #######################################################################################################################



// ##################################################### User Function Definitions #######################################

	
	
	// Working function for the threads:
	void *do_work(void *arg)
	{
	
		// Typecasting received argument pointer
		// to T_Arg type:
		T_Args *arg_pointer;
		arg_pointer = (T_Args*) arg;		
		
		// Unpacking the content of
		// the argument object into
		// local variables:
		Baker_Thread_Type btt = arg_pointer->btt;
		int baker_id = 	arg_pointer->baker_id;
			
		pthread_mutex_t *left_lock = arg_pointer->left_lock;
		pthread_mutex_t *right_lock = arg_pointer->right_lock;
		
		pthread_cond_t *left_handed_mitt_available = arg_pointer->left_handed_mitt_available;
		pthread_cond_t *right_handed_mitt_available = arg_pointer->right_handed_mitt_available;
		
		int *left_handed_mitts = arg_pointer->left_handed_mitts;
		int *right_handed_mitts = arg_pointer->right_handed_mitts;
		
		
		// Utility variables:
		int iteration = 0;
		int num_usec;
		double lbound_sec = 0.2;
		double ubound_sec = 0.5;

		double total_wait_time = 0.0; // Thread's total wait time
		struct timeval timestamp_a,timestamp_b; // Struct object for getting timestamps

		
		while(iteration < NUM_ITERATIONS)
		{
			
			// It's a lefty baker:
			if(btt == Lefty)
			{
				// Work:
					fprintf(stdout,"[Left-handed baker %d] is working...\n",baker_id);
					fflush(stdout);
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);			
				
				// Get oven mitts:
					fprintf(stdout,"[Left-handed baker %d] wants a left-handed mitt...\n",baker_id);
					fflush(stdout);

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);
					
					// Acquire lock:
					pthread_mutex_lock(left_lock);
					
					while(*(left_handed_mitts) == 0)
					{
						pthread_cond_wait(left_handed_mitt_available,left_lock);
					}

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);					

					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;
					
					// Get the mitt
					(*left_handed_mitts)--;
					
					// Release lock:
					pthread_mutex_unlock(left_lock);
					fprintf(stdout,"[Left-handed baker %d] has got a left-handed mitt...\n",baker_id);
					fflush(stdout);				
				
				// Put cookies in the oven:
				
					fprintf(stdout,"[Left-handed baker %d] has put cookies in the oven and waiting...\n",baker_id);
					fflush(stdout);
				
				// Wait for cookies to be baked:
				
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);
				
				// Remove cookies from the oven:
					fprintf(stdout,"[Left-handed baker %d] has taken cookies out of the oven...\n",baker_id);
					fflush(stdout);					
				
				// Put oven mitts back:

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);
					
					// Acquire lock:
					pthread_mutex_lock(left_lock);

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);					

					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;
					
					// Putting back the oven mitt:
					(*left_handed_mitts)++;
					fprintf(stdout,"[Left-handed baker %d] has put back a left-handed mitt...\n",baker_id);
					fflush(stdout);		
					
					// Send signal:
					pthread_cond_signal(left_handed_mitt_available);					
					// Release lock:
					pthread_mutex_unlock(left_lock);
			}
			
			// It's a righty baker:
			else if(btt == Righty)
			{
				// Work:
					fprintf(stdout,"[Right-handed baker %d] is working...\n",baker_id);
					fflush(stdout);
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);			
				
				// Get oven mitts:
					fprintf(stdout,"[Right-handed baker %d] wants a right-handed mitt...\n",baker_id);
					fflush(stdout);

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);
					
					// Acquire lock:
					pthread_mutex_lock(right_lock);
					
					while(*(right_handed_mitts) == 0)
					{
						pthread_cond_wait(right_handed_mitt_available,right_lock);
					}

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);

					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;
					
					
					// Get the mitt
					(*right_handed_mitts)--;
					
					// Release lock:
					pthread_mutex_unlock(right_lock);
					fprintf(stdout,"[Right-handed baker %d] has got a right-handed mitt...\n",baker_id);
					fflush(stdout);				
				
				// Put cookies in the oven:
				
					fprintf(stdout,"[Right-handed baker %d] has put cookies in the oven and waiting...\n",baker_id);
					fflush(stdout);
				
				// Wait for cookies to be baked:
				
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);
				
				// Remove cookies from the oven:
					fprintf(stdout,"[Right-handed baker %d] has taken cookies out of the oven...\n",baker_id);
					fflush(stdout);	
								
				// Put oven mitts back:

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);
					
					// Acquire lock:
					pthread_mutex_lock(right_lock);

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);

					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;
					
					// Putting back the oven mitt:
					(*right_handed_mitts)++;
					fprintf(stdout,"[Right-handed baker %d] has put back a right-handed mitt...\n",baker_id);
					fflush(stdout);			
					
					// Send signal:
					pthread_cond_signal(right_handed_mitt_available);					
					// Release lock:
					pthread_mutex_unlock(right_lock);
			
			}
			
			// It's a cautious baker:
			else
			{
				// Work:
					fprintf(stdout,"[Cautious baker %d] is working...\n",baker_id);
					fflush(stdout);
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);			
				
				// Get oven mitts:
					
					
					// Left-hand first:
					
					fprintf(stdout,"[Cautious baker %d] wants a left-handed mitt...\n",baker_id);
					fflush(stdout);

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);

					// Acquire lock:
					pthread_mutex_lock(left_lock);
					
					while(*(left_handed_mitts) == 0)
					{
						pthread_cond_wait(left_handed_mitt_available,left_lock);
					}					
					// Get the mitt
					(*left_handed_mitts)--;					
					// Release lock:
					pthread_mutex_unlock(left_lock);
					fprintf(stdout,"[Cautious baker %d] has got a left-handed mitt...\n",baker_id);
					fflush(stdout);
					
					// Now a right-hand one:
					
					fprintf(stdout,"[Cautious baker %d] wants a right-handed mitt...\n",baker_id);
					fflush(stdout);					
					// Acquire lock:
					pthread_mutex_lock(right_lock);
					
					while(*(right_handed_mitts) == 0)
					{
						pthread_cond_wait(right_handed_mitt_available,right_lock);
					}

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);

					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;

					// Get the mitt
					(*right_handed_mitts)--;					
					// Release lock:
					pthread_mutex_unlock(right_lock);
					fprintf(stdout,"[Cautious baker %d] has got a right-handed mitt...\n",baker_id);
					fflush(stdout);
									
				
				// Put cookies in the oven:
				
					fprintf(stdout,"[Cautious baker %d] has put cookies in the oven and waiting...\n",baker_id);
					fflush(stdout);
				
				// Wait for cookies to be baked:
				
					// Random sleep:
					num_usec = lbound_sec*1000000 + (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
		   			usleep(num_usec);
				
				// Remove cookies from the oven:
					fprintf(stdout,"[Cautious baker %d] has taken cookies out of the oven...\n",baker_id);
					fflush(stdout);	
								
				// Put oven mitts back:
					
					
					// Put back the left-handed mitt:

					// Get timestamp:
					gettimeofday(&timestamp_a,NULL);
					
					// Acquire lock:
					pthread_mutex_lock(left_lock);
					
					// Putting back the oven mitt:
					(*left_handed_mitts)++;
					fprintf(stdout,"[Cautious baker %d] has put back a left-handed mitt...\n",baker_id);
					fflush(stdout);			
					
					// Send signal:
					pthread_cond_signal(left_handed_mitt_available);					
					// Release lock:
					pthread_mutex_unlock(left_lock);
					
					
					
					// Put back the right-handed mitt:
					
					// Acquire lock:
					pthread_mutex_lock(right_lock);

					// Get timestamp:
					gettimeofday(&timestamp_b,NULL);
					// Update total wait time:
					total_wait_time += ( 1000000.0*(timestamp_b.tv_sec - timestamp_a.tv_sec) + (timestamp_b.tv_usec - timestamp_a.tv_usec))/1000000.0;
					
					// Putting back the oven mitt:
					(*right_handed_mitts)++;
					fprintf(stdout,"[Cautious baker %d] has put back a right-handed mitt...\n",baker_id);
					fflush(stdout);			
					
					// Send signal:
					pthread_cond_signal(right_handed_mitt_available);					
					// Release lock:
					pthread_mutex_unlock(right_lock);
				
			
			}
		
			// Increment the iteration:
			iteration++;
		
		}		
		
		// Update the argument objects thread
		// wait time field:
		arg_pointer->thread_wait_time = total_wait_time;
		
		
		return NULL;	
	
	
	}
	
// ################################################# End of Program ######################################################








































