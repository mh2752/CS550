#!/bin/bash
#SBATCH --job-name=cs550_A6_q1_p1  # the name of your job
#SBATCH --output=/scratch/mh2752/cs550_a6_q1_p1.out # this is the file your output and errors go to
#SBATCH --time=00:05:00		# 5 min, shorter time, quicker start
#SBATCH --mem=2048         #memory requested
#SBATCH -G 1 #resource requirement
#SBATCH --qos=gpu_class 
#SBATCH --constraint k80 # Generation of the GPU	       


# load cuda
module load cuda


srun hostname
srun nvidia-smi


echo "----------------- Epsilon = 5.0 ----------------------------------------------------------\n"
for iteration in 1 2 3
do
    #compile your application
    nvcc -arch=compute_37 -code=sm_37 -lcuda -Xcompiler -fopenmp  question1_p1.cu -o q1p1k80

    # run your application, precede the application command with srun
    srun ./q1p1k80 5.0 #
done


echo "----------------- Epsilon = 10.0 ----------------------------------------------------------\n"
for iteration in 1 2 3
do
    #compile your application
    nvcc -arch=compute_37 -code=sm_37 -lcuda -Xcompiler -fopenmp  question1_p1.cu -o q1p1k80

    # run your application, precede the application command with srun
    srun ./q1p1k80 10.0 #
done



