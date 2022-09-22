<<comment

Bash script for compiling and running 
crazy_scientist_v4_mh2752.c program
10 times.

Author:
	Nazmul(mh2752)

comment


#!/bin/bash


for i in 1 2 3 4 5 6 7 8 9 10
do
	echo "Time trial $i:"
	gcc -fopenmp crazy_scientist_v4_mh2752.c -lm -o crazy_scientist_v4
	./crazy_scientist_v4
done
