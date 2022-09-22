<<comment

		This is a simple bash shell script for running 
		the Question-1 solution.

		It compiles and runs the program 3 times first 
		without using the -O3 compiler flag and reports
		the average response times for epsilon = 5. 

		Then it will compile and run the program 3 times 
		with using the -O3 compiler flag and report the 
		average response time for epsilon = 5.


		It then repeats the two phases above for epsilon = 10.

    *** You have to change the number of threads in the source
		code files and then again run this bash script to get
		the reponse times for sequential/parallel versions.

comment



declare -a CONSOLE_OUTPUTS_NO_O3_E5
declare -a CONSOLE_OUTPUTS_WITH_O3_E5

declare -a CONSOLE_OUTPUTS_NO_O3_E10
declare -a CONSOLE_OUTPUTS_WITH_O3_E10

question=$"question1_mh2752.c"
seq_or_para="sequential"


for index in 0 1 2
do
	gcc -fopenmp $question -lm -o q1
	CONSOLE_OUTPUTS_NO_O3_E5[$index]=$(./q1 5.0)
done




for index in 0 1 2
do
	gcc -fopenmp $question -O3 -lm -o q1
	CONSOLE_OUTPUTS_WITH_O3_E5[$index]=$(./q1 5.0)
done




for index in 0 1 2
do
	gcc -fopenmp $question -lm -o q1
	CONSOLE_OUTPUTS_NO_O3_E10[$index]=$(./q1 10.0)
done



for index in 0 1 2
do
	gcc -fopenmp $question -O3 -lm -o q1
	CONSOLE_OUTPUTS_WITH_O3_E10[$index]=$(./q1 10.0)
done


<<comment2
------------------------------------------------------- Calculating Average Response Times --------------------------------------------
comment2


printf "Question-1 Response Times (epsilon =5, W/O -O3):\n"
for index in 0 1 2
do
	printf "Trial-$index\n"
	printf "${CONSOLE_OUTPUTS_NO_O3_E5[$index]}\n"
done

printf "\n\n"

<<comment2
-------------Averaging response times of the brute-force implementation
-------------with no -O3 and epsilon = 5
comment2

sum=0
for index in 0 1 2
do
	raw_line=${CONSOLE_OUTPUTS_NO_O3_E5[$index]}
	words=(${raw_line//"\n"/ })
	a=$(echo $"${words[8]} + 0" | bc -l)	
	sum=$(echo $"$sum+$a" | bc -l)	
done

average=$(echo $"$sum/3.0" | bc -l)

printf "Brute-force(no -O3, epsilon=5, $seq_or_para) response time (s): Total = $sum, Average = $average\n\n"


<<comment
-----------------------------------------------------------------------------------------------------------
comment




printf "Question-1 Response Times (epsilon = 5, With -O3):\n"
for index in 0 1 2
do
	printf "Trial-$index\n"
	printf "${CONSOLE_OUTPUTS_WITH_O3_E5[$index]}\n"
done

printf "\n\n"


<<comment2
-------------Averaging response times of the brute-force implementation
-------------with -O3 and epsilon = 5
comment2

sum=0
for index in 0 1 2
do
	raw_line=${CONSOLE_OUTPUTS_WITH_O3_E5[$index]}
	words=(${raw_line//"\n"/ })
	a=$(echo $"${words[8]} + 0" | bc -l)	
	sum=$(echo $"$sum+$a" | bc -l)	
done

average=$(echo $"$sum/3.0" | bc -l)

printf "Brute-force(with -O3, epsilon=5, $seq_or_para) response time (s): Total = $sum, Average = $average\n\n"

<<comment
-----------------------------------------------------------------------------------------------------------
comment


printf "Question-1 Response Times (epsilon =10, W/O -O3):\n"
for index in 0 1 2
do
	printf "Trial-$index\n"
	printf "${CONSOLE_OUTPUTS_NO_O3_E10[$index]}\n"
done

printf "\n\n"

<<comment2
-------------Averaging response times of the brute-force implementation
-------------with no -O3 and epsilon = 10
comment2

sum=0
for index in 0 1 2
do
	raw_line=${CONSOLE_OUTPUTS_NO_O3_E10[$index]}
	words=(${raw_line//"\n"/ })
	a=$(echo $"${words[8]} + 0" | bc -l)	
	sum=$(echo $"$sum+$a" | bc -l)	
done

average=$(echo $"$sum/3.0" | bc -l)

printf "Brute-force(no -O3, epsilon=10, $seq_or_para) response time (s): Total = $sum, Average = $average\n"

<<comment
-----------------------------------------------------------------------------------------------------------
comment



printf "Question-1 Response Times (epsilon =10, With -O3):\n"
for index in 0 1 2
do
	printf "Trial-$index\n"
	printf "${CONSOLE_OUTPUTS_WITH_O3_E10[$index]}\n"
done

printf "\n\n"

<<comment2
-------------Averaging response times of the brute-force implementation
-------------with -O3 and epsilon = 10
comment2

sum=0
for index in 0 1 2
do
	raw_line=${CONSOLE_OUTPUTS_WITH_O3_E10[$index]}
	words=(${raw_line//"\n"/ })
	a=$(echo $"${words[8]} + 0" | bc -l)	
	sum=$(echo $"$sum+$a" | bc -l)	
done

average=$(echo $"$sum/3.0" | bc -l)

printf "Brute-force(with -O3, epsilon=10, $seq_or_para) response time (s): Total = $sum, Average = $average\n"

<<comment
-----------------------------------------------------------------------------------------------------------
comment