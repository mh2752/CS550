/*

	CS-550(Fall-2021)

		Assignment 5: Question 2

	Author:

		Nazmul(mh2752)

*/


//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "omp.h"


// Sublime Text key-board shortcut (Ubuntu)

	//[Hit "Ctl+Shift+]" to unfold]
	//[Hit "Ctl+Shift+[" to fold]


// ----------------------------------------------- All Macro Definitions ---------------------------------------------------------



//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72
// Controlling the number of OMP threads:
#define NUM_THREADS 1




// ----------------------------------------------- All Struct Definitions -------------------------------------------------------- 



// Struct for the 2D points:
typedef struct pointData
{
	double x;
	double y;

} Point2D;


// Struct for unique x valued objects:
typedef struct Unique_X
{

	// The unique x-value of this
	// point object of the grid:
	double x_value;

	// Total number of y-values
	// associated with this unique
	// x-valued object who represent
	// a collection of 2D points on
	// the cartesian grid:
	int num_of_y_values;

	// Array pointer holding all the 
	// y-values:
	double *all_y_values;
	

	// Number of left and right Unique_X 
	// neighbors within 'epsilon' distance
	// of this Unique_X object:
	int num_left_neighbors;
	int num_right_neighbors;

	// Array pointers storing grid indices
	// of neighboring Unique_X objects:
	int *left_neighbor_indices;	
	int *right_neighbor_indices;
	


} Unique_X;





// ---------------------------------------------- All Function Prototype Declarations -------------------------------------

void generateDataset(struct pointData * data);
int compare_doubles(const void *value_a,const void *value_b);
int get_number_of_yvalues_within_limits(double y_upper,double y_lower,const double *y_array,int y_array_length);


// ------------------------------------------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	
	
	//Read epsilon distance from command line
	if (argc!=2)
	{
	printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
	return 0;
	}
	
	
	char inputEpsilon[20];
	strcpy(inputEpsilon,argv[1]);
	double epsilon=atof(inputEpsilon);



	//generate dataset:
	Point2D *data;
	
	data=(Point2D*)malloc(sizeof(Point2D)*N);
	
	printf("Size of dataset (MiB): %f\n",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	
	// Calling the data genration
	// function:
	generateDataset(data);


	//change OpenMP settings:
	omp_set_num_threads(NUM_THREADS);


	double tstart=omp_get_wtime();


	//Write your code here:
	//The data you need to use is stored in the variable "data", 
	//which is of type pointData



	// -------------------------------------- Step-by-Step Implementation of the Transfromed Algorithm ----------------------

		/* Step-1: Sort the array of Point2D objects based on the
				   objects' x values. Use qsort() library function
				   available in C. It seems to hold well against 
				   average case scenarios.// Put the object in the array:
						grid[grid_index] = ux;
		*/				
				qsort(data,N,sizeof(Point2D),compare_doubles);


				/*
				for(int index = 0; index < N; index++)
				{// Put the object in the array:
						grid[grid_index] = ux;
					printf("\nPoint-%d: x = %lf, y = %lf",index,data[index].x,data[index].y);
				}*/



		/* Step-2: Find the total number of Point2D objects with
				   unique x-values. This will be the total number
				   of Unique_X struct objects.
		*/		   
				int num_of_unique_x = 0;

				for(int index = 0; index < N; index++)
				{
					if(index == 0)
					{
						num_of_unique_x++;
					}
					else
					{	
						// Current value is different than the
						// value immediately before it. Increment
						// the unique x count:
						if(data[index-1].x != data[index].x)
						{
							num_of_unique_x++;
						}

					}
				}


				//printf("\nNum of Unique X values = %d",num_of_unique_x);


		/* Step-3: Allocate memory for storing all Unique_X struct objects
				   and populate the array.
		*/

				Unique_X *grid; // The pointer
				grid = (Unique_X*)malloc(sizeof(Unique_X)*num_of_unique_x); // Memory allocation


				// Traverse the data array and populate & update grid array
				// appropriately:
				for(int data_index = 0, grid_index=0; data_index < N; data_index++)
				{


					// Very first iteration:
					if(data_index == 0)
					{	
						// Create a Unique_X object:
						Unique_X ux;

						// Initialize it's appropriate
						// member variables:
						ux.x_value = data[data_index].x;
						ux.num_of_y_values = 1;

						// Put the object in the array:
						grid[grid_index] = ux;
						
					}
					else
					{	
						// Current value is different than the
						// value immediately before it. Another
						// unique x-value:
						if(data[data_index-1].x != data[data_index].x)
						{
							// Increment grid_index:
							grid_index++;

							// Create Unique_X object:
							Unique_X ux;

							// Initialize it's appropriate
							// member variables:
							ux.x_value = data[data_index].x;
							ux.num_of_y_values = 1;

							// Put the object in the array:
							grid[grid_index] = ux;
						}


						// Same x-value as the previous one.
						// Just increment the num_of_y_values
						// variable.
						else
						{
							grid[grid_index].num_of_y_values++;

						}

					}
				}


				/*
				for(int index = 0; index < num_of_unique_x; index++)
				{
					printf("\nGrid Point-%d: x_value = %lf, num_of_y_values = %d",index,grid[index].x_value,grid[index].num_of_y_values);
				}*/

		/* Step-4: Allocate memories for all_y_values of each Unique_X
				   object in the 'grid' array.

		*/
				// Allocate memories:
				for(int index = 0; index < num_of_unique_x; index++)
				{
					grid[index].all_y_values = (double*)malloc(sizeof(double)*grid[index].num_of_y_values);
				}

				
				// Populate the arrays:

				// Traverse the data array and populate & update grid array's
				// all_y_values arrays appropriately:

				for(int data_index = 0, grid_index=0,index_to_insert_y_value; data_index < N; data_index++)
				{


					// Very first iteration:
					if(data_index == 0)
					{		

						// Index in the all_y_values array
						// where the y_value is to be placed:
						index_to_insert_y_value = 0;

						// Put the y-value in the Unique_X 
						// object's all_y_values array:
						grid[grid_index].all_y_values[index_to_insert_y_value] = data[data_index].y;
						
					}
					else
					{	
						// Current value is different than the
						// value immediately before it. Another
						// unique x-value:
						if(data[data_index-1].x != data[data_index].x)
						{
							// Increment grid_index:
							grid_index++;

							// Reset index_to_insert_y_value to zero:
							index_to_insert_y_value = 0;

							// Put the y-value in the Unique_X 
							// object's all_y_values array:
							grid[grid_index].all_y_values[index_to_insert_y_value] = data[data_index].y;						

						}


						// Same x-value as the previous one.
						// Just increment the index_to_insert_y_value
						// variable and insert the y_value:
						else
						{
							index_to_insert_y_value++;

							grid[grid_index].all_y_values[index_to_insert_y_value] = data[data_index].y;
						}

					}
				}


				/*
				for(int index = 0; index < num_of_unique_x; index++)
				{
					printf("\nGrid Point-%d: x_value = %lf, num_of_y_values = %d",index,grid[index].x_value,grid[index].num_of_y_values);
					printf("\n\tAll y values at this grid point:\n\t\t");
					for(int index2 = 0; index2 < grid[index].num_of_y_values; index2++)
					{
						printf("%lf   ",grid[index].all_y_values[index2]);
					}
				}*/

		/* Step-5: Sort each all_y_values array in the grid array:
		*/

				for(int grid_index; grid_index < num_of_unique_x; grid_index++)
				{
					qsort(grid[grid_index].all_y_values,grid[grid_index].num_of_y_values,sizeof(double),compare_doubles);
				}

				/*
				for(int index = 0; index < num_of_unique_x; index++)
				{
					printf("\nGrid Point-%d: x_value = %lf, num_of_y_values = %d",index,grid[index].x_value,grid[index].num_of_y_values);
					printf("\n\tAll y values at this grid point:\n\t\t");
					for(int index2 = 0; index2 < grid[index].num_of_y_values; index2++)
					{
						printf("%lf   ",grid[index].all_y_values[index2]);
					}
				}*/			

		/* Step-6: Calculate and set each Unique_X object's number
				   of left and right neighboring Unique_X objects
				   within the epsilon distance.

				   Then allocate the required memories and pouplate
				   the arrays.
		*/

				
				int left,right; // Utility variables.
				double left_limit,right_limit; // Utility variables.

				
				// Updating neighbor counts:

				for(int grid_index = 0; grid_index < num_of_unique_x; grid_index++)
				{
					left = grid_index-1; // Left index of this object in the sorted 'grid' array.
					right = grid_index+1; // Right index of this object in the sorted 'grid' array.

					// Initialize left and right neighbors
					// of the Unique_X object to zero:
					grid[grid_index].num_left_neighbors = 0;
					grid[grid_index].num_right_neighbors = 0;


					// Find the left and right limits:
					left_limit = grid[grid_index].x_value - epsilon;
					right_limit = grid[grid_index].x_value + epsilon;


					// Traverse left and update:
					while( (left>=0) && (grid[left].x_value >= left_limit) )
					{
						grid[grid_index].num_left_neighbors++;
						left--;
					}


					// Traverse right and update:
					while( (right<num_of_unique_x) && (grid[right].x_value <= right_limit) )
					{
						grid[grid_index].num_right_neighbors++;
						right++;
					}
				}


				/*
				for(int index = 0; index < num_of_unique_x; index++)
				{
					printf("\nGrid Point-%d: x_value = %lf",index,grid[index].x_value);
					printf("\n\tNum of rigth neighbors = %d",grid[index].num_right_neighbors);
					printf("\n\tNum of left neighbors = %d",grid[index].num_left_neighbors);
				}*/


				// Allocate Memories:
				for(int grid_index = 0; grid_index < num_of_unique_x; grid_index++)
				{	
					// Allocate memory against the pointers:
					grid[grid_index].left_neighbor_indices = (int*)malloc(sizeof(int)*grid[grid_index].num_left_neighbors);
					grid[grid_index].right_neighbor_indices = (int*)malloc(sizeof(int)*grid[grid_index].num_right_neighbors);
				}


				// Populate the neighbor indices arrays:
				for(int grid_index = 0; grid_index < num_of_unique_x; grid_index++)
				{
					left = grid_index-1; // Left index of this object in the sorted 'grid' array.
					right = grid_index+1; // Right index of this object in the sorted 'grid' array.					


					// Find the left and right limits:
					left_limit = grid[grid_index].x_value - epsilon;
					right_limit = grid[grid_index].x_value + epsilon;


					// Traverse left and update:
					int temp_index = 0; // It should go upto

					while( (left>=0) && (grid[left].x_value >= left_limit) )
					{
						grid[grid_index].left_neighbor_indices[temp_index] = left;
						temp_index++;
						left--;
					}


					// Traverse right and update:
					temp_index = 0;
					while( (right<num_of_unique_x) && (grid[right].x_value <= right_limit) )
					{
						grid[grid_index].right_neighbor_indices[temp_index] = right;
						temp_index++;
						right++;
					}
				}


				/*
				for(int grid_index = 0; grid_index < num_of_unique_x; grid_index++)
				{
					printf("\nGrid Point-%d: x_value = %lf",grid_index,grid[grid_index].x_value);
					
					printf("\n\tNum of rigth neighbors = %d",grid[grid_index].num_right_neighbors);
					
					printf("\n\t\tRight neighbor indices: ");
					
					for(int index2 = 0; index2 < grid[grid_index].num_right_neighbors; index2++)
					{
						printf(" %d",grid[grid_index].right_neighbor_indices[index2]);
					}
					
					printf("\n");
					
					printf("\n\tNum of left neighbors = %d",grid[grid_index].num_left_neighbors);
					
					printf("\n\t\tLeft neighbor indices: ");
					
					for(int index2 = 0; index2 < grid[grid_index].num_left_neighbors; index2++)
					{
						printf(" %d",grid[grid_index].left_neighbor_indices[index2]);
					}

					printf("\n");
				}*/


		/* Step-7: Now you have all the grid data ready. Go through each y-value in
				   each of the Unique_X object's all_y_values arrays and calculate
				   their number of neighboring 2D points.
		*/
				

				// Utility variables:
					double temp_x,temp_y; // Self(Unique_X)'s x-value and y-values 
					double *y_array_ptr; // Pointer to self's all_y_values
					int y_array_length; // Length of self's all_y_values
					
					int left_neighbors_length; // Number of left neighbor Unique_X
					int right_neighbors_length; // Number of right neighbor Unique_X
					int *left_neighbors; // Pointer for referencing left_neighbor_indices
					int *right_neighbors; // Pointer for referencing right_neighbor_indices
					double neighbors_x_value; // x-value of a neighbor Unique_X object:

					int num_of_neighbor_points = 0; // Neighbor count
					double y_upper,y_lower,delta_y;
					int grid_index;
					
				
				#pragma omp parallel for private(grid_index,temp_x,temp_y,y_array_ptr,y_array_length,left_neighbors_length,right_neighbors_length,left_neighbors,right_neighbors,neighbors_x_value,y_upper,y_lower,delta_y) shared(grid) schedule(dynamic) reduction(+:num_of_neighbor_points)
				for(grid_index = 0; grid_index < num_of_unique_x; grid_index++)
				{	

					// Get the x-value of this Unique_X object:
					temp_x = grid[grid_index].x_value;

					// Length of this Unique_X object's all_y_values:
					y_array_length = grid[grid_index].num_of_y_values;

					// Get the pointer to the Unique_X object's all_y_values:
					y_array_ptr = grid[grid_index].all_y_values;

					// Get the lengths of this Unique_X object's
					// left and right neighbor arrays:
					left_neighbors_length = grid[grid_index].num_left_neighbors;
					right_neighbors_length = grid[grid_index].num_right_neighbors;

					// Now get the pointers to these neighbor arrays:
					left_neighbors = grid[grid_index].left_neighbor_indices;
					right_neighbors = grid[grid_index].right_neighbor_indices;



					// Get the y-values of this Unique_X object
					// into temp_y one by one and count how many points lie
					// within the circle whose center is (temp_x,temp_y) with
					// a radius of 'epsilon'.
					for(int index = 0; index < y_array_length; index++)
					{	
						// Get the y-value:
						temp_y = y_array_ptr[index];

						// 1: Check self first:

							// Get the upper and lower limits
							// for the y-value:
							y_upper = temp_y + epsilon;
							y_lower = temp_y - epsilon;

							// Call the utility function and
							// update the count:
							num_of_neighbor_points += get_number_of_yvalues_within_limits(y_upper,y_lower,y_array_ptr,y_array_length);

						// 2: Check all left neighbors now:

							for(int index2 = 0; index2 < left_neighbors_length; index2++)
							{
								// Get neighbor's x-value:
								neighbors_x_value = grid[left_neighbors[index2]].x_value;

								// Find where this 'x = neighbors_x_value' vertical line
								// intersects the circle with centre at (temp_x,temp_y)
								// and radius = epsilon:
								delta_y = sqrt(pow(epsilon,2) - pow(temp_x - neighbors_x_value,2));

								// This 'x = neighbors_x_value' vertical line
								// is a tangent to the circle:
								if(delta_y == 0)
								{
									// If the temp_y exists in this neighbor
									// object's all_y_values, increment the 
									// neighbor count by 1.
									//
									// Peform binary search on the array:
									
									int l,r,m;

									l = 0;
									r = grid[left_neighbors[index2]].num_of_y_values;

									bool is_found = false;
									bool loop_controller = (l <= r) && (!is_found);
									

									while(loop_controller)
									{
										m = (int)((l+r)/2);

										if(grid[left_neighbors[index2]].all_y_values[m] > temp_y)
										{
											r = m-1;
										}
										else if(grid[left_neighbors[index2]].all_y_values[m] < temp_y)
										{
											l = m+1;
										}
										else
										{
											is_found = true;
										}

										loop_controller = (l <= r) && (!is_found);
									}

									if(is_found)
									{
										num_of_neighbor_points++;
									}

								}


								// 
								else
								{
									
									// Get the upper and lower limits:
									y_upper = temp_y + delta_y;
									y_lower = temp_y - delta_y;

									// Call the utility function and
									// update the neighbor count:
									num_of_neighbor_points += get_number_of_yvalues_within_limits(y_upper,y_lower, grid[left_neighbors[index2]].all_y_values, grid[left_neighbors[index2]].num_of_y_values);

								}
							} // Checking for left neighbors done


						// 3: Check all right neighbors:

							for(int index2 = 0; index2 < right_neighbors_length; index2++)
							{
								// Get neighbor's x-value:
								neighbors_x_value = grid[right_neighbors[index2]].x_value;

								// Find where this 'x = neighbors_x_value' vertical line
								// intersects the circle with centre at (temp_x,temp_y)
								// and radius = epsilon:
								delta_y = sqrt(pow(epsilon,2) - pow(neighbors_x_value-temp_x,2));

								// This 'x = neighbors_x_value' vertical line
								// is a tangent to the circle:
								if(delta_y == 0)
								{
									// If the temp_y exists in this neighbor
									// object's all_y_values, increment the 
									// neighbor count by 1.
									//
									// Peform binary search on the array:
									
									int l,r,m;

									l = 0;
									r = grid[right_neighbors[index2]].num_of_y_values;

									bool is_found = false;
									bool loop_controller = (l <= r) && (!is_found);
									

									while(loop_controller)
									{
										m = (int)((l+r)/2);

										if(grid[left_neighbors[index2]].all_y_values[m] > temp_y)
										{
											r = m-1;
										}
										else if(grid[left_neighbors[index2]].all_y_values[m] < temp_y)
										{
											l = m+1;
										}
										else
										{
											is_found = true;
										}

										loop_controller = (l <= r) && (!is_found);
									}

									if(is_found)
									{
										num_of_neighbor_points++;
									}

								}


								// 
								else
								{
									
									// Get the upper and lower limits:
									y_upper = temp_y + delta_y;
									y_lower = temp_y - delta_y;

									// Call the utility function and
									// update the neighbor count:
									num_of_neighbor_points += get_number_of_yvalues_within_limits(y_upper,y_lower, grid[right_neighbors[index2]].all_y_values, grid[right_neighbors[index2]].num_of_y_values);

								}
							} // Done checking right neighbors

					}

				}
				   

	

	



	double tend=omp_get_wtime();
	
	
	
	printf("Total time (s): %f\n",tend-tstart);
	
	printf("Total points within epsilon: %d\n",num_of_neighbor_points);



	// Free data memories:
	free(data);


	// De-allocating grid memories:

		// First, free all_y_values
		for(int grid_index = 0; grid_index < num_of_unique_x; grid_index++)
		{
			free(grid[grid_index].all_y_values);
			free(grid[grid_index].left_neighbor_indices);
			free(grid[grid_index].right_neighbor_indices);

		}

		// Now, free grid
		free(grid);
	


	printf("\n");
	return 0;
 }










// ******************************************** All Utility Functions **************************************************************************************





// Function for generating Point2D data.
//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{

	//seed RNG
	srand(SEED);


	for (unsigned int i=0; i<N; i++){
		data[i].x=1000.0*((double)(rand()) / RAND_MAX);	
		data[i].y=1000.0*((double)(rand()) / RAND_MAX);	
	}
	

}




//Function to be used for the qsort() library
// function:
int compare_doubles(const void *value_a,const void *value_b)
{

	double first = *((double*)value_a);
	double second = *((double*)value_b);

	if (first > second)
		return 1;

	else if(first < second)
		return -1;

	else
		return 0;

}



// A function that returns the number of y-values
// from an array of y-values within a given limit
// (inclusive). Worst case complexity is 2*O(logN)
int get_number_of_yvalues_within_limits(double y_upper,double y_lower,const double *y_array,int y_array_length)
{

	
	// No items in the array.
	// Return 0:
	if(y_array_length == 0)
	{
		return 0;
	}

	// Given lower limit is greater than the
	// max value in the array. No y-value will
	// fall within the range.
	else if(y_lower > y_array[y_array_length-1])
	{
		return 0;
	}

	// Given upper limit is smaller than the
	// min value in the array. No y-value will
	// fall within the range.
	else if(y_upper < y_array[0])
	{
		return 0;
	}



	// ----- If execution reaches here, that means there might be
	// 		 some y-values in the array within the given upper and
	//		 lower limitis.
	//
	//		 Utilize the binanry search method to find out the
	//       number of values in the given range (inclusive).


	// Utility variables for performing
	// a modified version of the binary 
	// search. 
	// 
	// Upon failure to find a key,
	// it returns the left and right index
	// of the place where the missing item
	// should have been situated in the
	// sorted y_array.

	int left_index,right_index,mid_index;
	bool loop_controller,is_found;

	// These shall be used in counting
	// the 
	int lower_index = -1;
	int upper_index = -1; 


	// Initializing the indices:
	left_index = 0;
	right_index = (y_array_length -1);


	// Initializing the booleans:	
	is_found = false;
	loop_controller = (left_index <= right_index) && (!is_found);

	// Search for the lower
	// limit of the range:
	while(loop_controller)
	{
		mid_index = (int)((left_index+right_index)/2);

		if(y_array[mid_index] > y_lower)
		{
			// Search left half:
			right_index = mid_index-1;
		}
		else if(y_array[mid_index] < y_lower)
		{
			// Search right half:
			left_index = mid_index+1;
		}

		// Item found at mid_index
		else
		{
			is_found = true;
		}

		// Updating loop controller boolean:
		loop_controller = (left_index<=right_index) && (!is_found);		

	}

	if(is_found)
	{
		lower_index = mid_index;
	}

	// Item is not found.
	// Item should've been between
	// (right_index,left_index)
	else
	{	
		
		lower_index = left_index;
		
	}



	// Search for the upper limit:

	// Initializing the indices:
	left_index = 0;
	right_index = (y_array_length -1);

	// Initializing the booleans:	
	is_found = false;
	loop_controller = (left_index <= right_index) && (!is_found);

	

	while(loop_controller)
	{
		mid_index = (int)((left_index+right_index)/2);

		if(y_array[mid_index] > y_upper)
		{
			// Search left half:
			right_index = mid_index-1;
		}
		else if(y_array[mid_index] <y_upper)
		{
			// Search right half:
			left_index = mid_index+1;
		}

		// Item found at mid_index
		else
		{
			is_found = true;
		}

		// Updating loop controller boolean:
		loop_controller = (left_index<=right_index) && (!is_found);		

	}

	if(is_found)
	{
		upper_index = mid_index;
	}

	// Item is not found.
	// Item should've been between
	// (right_index,left_index)
	else
	{			
		upper_index = right_index; // Becasue left_index <= right_index is false			
	}	
	
	return (upper_index-lower_index+1);

}
