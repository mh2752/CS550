/*

	CS-550(Fall-2021)

		Assignment 5: Question 1

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
#include <string.h>
#include <math.h>
#include "omp.h"

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72


// Controlling the number of OMP threads:
#define NUM_THREADS 1


typedef struct pointData
{
	double x;
	double y;
} Point2D;



void generateDataset(struct pointData * data);


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
	Point2D * data;
	data=(Point2D*)malloc(sizeof(Point2D)*N);
	printf("Size of dataset (MiB): %f\n",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	generateDataset(data);

	




	//change OpenMP settings:
	omp_set_num_threads(NUM_THREADS);


	double tstart=omp_get_wtime();
	

	//Write your code here:
	//The data you need to use is stored in the variable "data", 
	//which is of type pointData
	
	
	// Brute force, parallel (Sequential when NUM_THREADS == 1):

		// row and column indices:
		int row,col;

		// Count of points within epsilon:
		int points_within_epsilon = 0;

		// Utility variable for distance:
		double distance;
		double x_0,y_0,x_temp,y_temp;


		#pragma omp parallel for private(row,col,distance,x_0,y_0,x_temp,y_temp) shared(data) schedule(dynamic) reduction(+:points_within_epsilon)
		for(row = 0; row < N; row++)
		{
			// Get the x & y coordinates of the point at index = row:
			//double x_0,y_0;
			x_0 = data[row].x;
			y_0 = data[row].y;		


			for(col = 0; col < N; col++)
			{
				//double x_temp,y_temp;

				x_temp = data[col].x;
				y_temp = data[col].y;

				distance = sqrt(pow(x_0 - x_temp,2)+pow(y_0 - y_temp,2));
				
				if(distance<=epsilon)
				{
					points_within_epsilon++;
				}
			}
		}
	
	

	double tend=omp_get_wtime();
	
	
	
	printf("Total time (s): %f\n",tend-tstart);
	printf("Total points within epsilon: %d\n",points_within_epsilon);


	free(data);
	return 0;
}


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
