/*

    CS-550 (Fall-2021)

    Assignment-7 : Question 1 Problem 1

    Author:

        Nazmul (mh2752@nau.edu)

*/


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "omp.h"
#include <iostream>
using namespace std;



// **********************************************************  Macro & Misc. Declarations ************************************************************


        // Number of datapoints in the
        // test dataset:
        #define N 100000

        // Seed for the random data
        // generator:
        #define SEED 72

        // For GPU implementation
        #define BLOCKSIZE 1024


        // Typedef for point data 
        // structure:
        typedef struct pointData
        {
            double x;
            double y;
        } pointData;


        // ------------------------------------------------- Prototype of data generation function ------------------------------------------
        void generateDataset(struct pointData * data);


        // ------------------------------------------------- Prototype of the function for warming up the GPU -------------------------------
        void warmUpGPU();

        // ------------------------------------------------- Prototype of the Kernel Function -----------------------------------------------
        __global__ void pointEpsilonBruteForce(pointData* all_data_points,int *device_points_within_epsilon,double *device_epsilon);



// *******************************************************************************************************************************************


int main(int argc, char *argv[])
{
	
	
	//Read epsilon distance from command line
	if (argc!=2)
	{
        printf("\n Incorrect number of input parameters. Please input an epsilon distance.\n");
        return 0;
	}
	
	
	char inputEpsilon[20];
	strcpy(inputEpsilon,argv[1]);
	double epsilon=atof(inputEpsilon);
	
	

	//generate dataset:
	pointData * data;
	data=(pointData*)malloc(sizeof(pointData)*N);
	printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	generateDataset(data);


	omp_set_num_threads(1);

	
    // Overall starting timestamp
    double tstart=omp_get_wtime();
	

	//Write your code here:
	//The data you need to use is stored in the variable "data", 
	//which is of type pointData:


        // Variale to store the total number of points within
        // the epsilon on the CPU:
        int hpwe;
        int *host_points_within_epsilon = &hpwe;
        *host_points_within_epsilon = 0;

        // Variable storing the count of the total number
        // of points within epsilon on the GPU:
        int *device_points_within_epsilon;

        // Array on the GPU for holding
        // the pointData objects:
        pointData *device_data;

        // Varaible for epsilon value
        // on the GPU:
        double *device_epsilon;


        // ----------------- Allocate memories on the GPU ------------------------------
        
            
            // Warming up the GPU first:
            warmUpGPU();           
            
            
            cudaError_t errorCode = cudaSuccess;
            
            
            
            // Allocate for the device_points_within_epsilon:
            errorCode = cudaMalloc((int **)&device_points_within_epsilon,sizeof(int));
            if (errorCode != cudaSuccess)
            {
                cout<<"Error: Memory allocation on GPU for total points count varaible failed!!!"<<endl;
            }


            // Allocate memory for the pointData array:
            errorCode = cudaMalloc((pointData**)&device_data,sizeof(pointData)*N);
            if (errorCode != cudaSuccess)
            {
                cout<<"Error: Memory allocation on GPU for PointData array failed!!!"<<endl;
            }


            // Allocate memory for epsilon value on the GPU:
            errorCode = cudaMalloc((double**)&device_epsilon,sizeof(double));
            if (errorCode != cudaSuccess)
            {
                cout<<"Error: Memory allocation on GPU for device_epsilon failed!!!"<<endl;
            }

            // Memcpy starting timestamp:
            double memcpy_tstart = omp_get_wtime();

            // Copy the info to the device (GPU):            
            errorCode=cudaMemcpy( device_points_within_epsilon, host_points_within_epsilon, sizeof(int), cudaMemcpyHostToDevice);
            if(errorCode != cudaSuccess) 
            {
                cout << "\nError: device_points_within_epsilon memcpy error with code " << errorCode << endl; 
            }

            // Copy device_data
            errorCode=cudaMemcpy( device_data, data, sizeof(pointData)*N, cudaMemcpyHostToDevice);
            if(errorCode != cudaSuccess) 
            {
                cout << "\nError: device_points_within_epsilon memcpy error with code " << errorCode << endl; 
            }

            // Copy epsilon data:
            errorCode=cudaMemcpy( device_epsilon, &epsilon, sizeof(double), cudaMemcpyHostToDevice);
            if(errorCode != cudaSuccess) 
            {
                cout << "\nError: device_epsilon memcpy error with code " << errorCode << endl; 
            }


            // Memcpy ending timestamp:
            double memcpy_tend = omp_get_wtime();



            // Initializing kernel properties:
            const unsigned int totalBlocks = ceil((N*1.0)/(1024.0));
            printf("\nTotal blocks in the kernel: %d",totalBlocks);


            // Getting kernel launch start time:
            double kernel_starttime = omp_get_wtime();

            // Launching the kernel:
            //vectorAdd<<<totalBlocks,1024>>>(dev_A, dev_B, dev_C);
            pointEpsilonBruteForce<<<totalBlocks,1024>>>(device_data,device_points_within_epsilon,device_epsilon);

            errorCode = cudaGetLastError();

            if(errorCode != cudaSuccess)
            {
		        cout<<"Error after kernel launch "<<errorCode<<endl;
	        }

             // Getting kernel launch start time:
            double kernel_endtime = omp_get_wtime();

            // Copy data from device to host 
            errorCode = cudaMemcpy(host_points_within_epsilon, device_points_within_epsilon, sizeof(int), cudaMemcpyDeviceToHost);

            double memcpy2_tend = omp_get_wtime();

            if(errorCode != cudaSuccess) 
            {
                cout << "\nError: getting result form GPU error with code " << errorCode << endl; 
            }

            // Synchronizing the devices:
            cudaDeviceSynchronize();
        
	
    // Overall ending timestamp
	double tend=omp_get_wtime();
	
    printf("\nTotal number of data-points generated: %d",N);
    printf("\nNumber of points within epsilon: %d",*host_points_within_epsilon);
	printf("\nTotal time (s): %f",tend-tstart);
    printf("\nData Transfer (Host-to-Device) Time: %f",memcpy_tend - memcpy_tstart);
    printf("\nData Transfer (Device-to-Host) Time: %f",memcpy2_tend - kernel_endtime);
    printf("\nKernel execution time: %f",kernel_endtime - kernel_starttime);    


	free(data);
	printf("\n");
	return 0;
}


// ************************************************ GPU Kernel Function Defintion ******************************************************************


__global__ void pointEpsilonBruteForce(pointData* all_data_points,int *device_points_within_epsilon,double *device_epsilon)
{

    // Obtaining the threads ID:
    unsigned int tid=threadIdx.x+ (blockIdx.x*blockDim.x);

    // Because, number of threads created
    // might be greater than N:
    if (tid >= N)
    {
	    return;
    }


    // Initializing self pointData object for
    // this thread:
    pointData my_point;
    
    my_point.x = all_data_points[tid].x;
    my_point.y = all_data_points[tid].y;


    // Varaible for keeping
    // points within this thread's
    // pointdata's epsilon range:
    int total_points_within_epsilon = 0;


    // Temporary pointData object:
    pointData temp;

    double distance;

    for(unsigned long int index = 0; index < N; index++)
    {
        // Obtaining the pointData object
        // at index's info:
        temp.x = all_data_points[index].x;
        temp.y = all_data_points[index].y;

        // Calculating the distance:
        distance = sqrt(pow(temp.x - my_point.x,2)+pow(temp.y - my_point.y,2));

        if(distance <= *device_epsilon)
        {
            total_points_within_epsilon++;
        }

    }

    /* Atomically adding the counts to the
    // global variable:
    for(unsigned long int i = 0; i < total_points_within_epsilon; i++)
    {
        atomicAdd(device_points_within_epsilon,1);    
    } */
    atomicAdd(device_points_within_epsilon,total_points_within_epsilon);


    return;

}


// ************************************************************* GPU Warmpup Routine **************************************************


__global__ void warmup(unsigned int * tmp) 
{
    if (threadIdx.x==0)
    {
        *tmp=555;
    }    

    return;
}



void warmUpGPU()
{

    printf("\nWarming up GPU for time trialing...\n");	

    unsigned int * dev_tmp;
    unsigned int * tmp;

    tmp=(unsigned int*)malloc(sizeof(unsigned int));

    *tmp=0;

    cudaError_t errorCode = cudaSuccess;

    errorCode = cudaMalloc((unsigned int**)&dev_tmp, sizeof(unsigned int));

    if(errorCode != cudaSuccess) 
    {
        cout << "\nError: dev_tmp error with code " << errorCode << endl; 
    }

    warmup<<<1,256>>>(dev_tmp);

    //copy data from device to host 
    errorCode=cudaMemcpy( tmp, dev_tmp, sizeof(unsigned int), cudaMemcpyDeviceToHost);
    if(errorCode != cudaSuccess) 
    {
        cout << "\nError: getting tmp result form GPU error with code " << errorCode << endl; 
    }

    printf("\ntmp (changed to 555 on GPU): %d",*tmp);

    cudaFree(dev_tmp);

    return;
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