#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void random_sleep(double a, double b);

#define NUM_ITERATIONS 10
#define NUM_LEFT_OVEN_MITTS 3
#define NUM_RIGHT_OVEN_MITTS 3

//Helper function to sleep a random number of microseconds
//picked between two bounds (provided in seconds)
//pass 0.2 and 0.5 into this function as arguments
void random_sleep(double lbound_sec, double ubound_sec) {
   int num_usec;
   num_usec = lbound_sec*1000000 + 
              (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
   usleep(num_usec);
   return; 
}


/*
 * Main function
 */
int main(int argc, char **argv) {

  int num_left_handed_bakers;
  int num_right_handed_bakers;
  int num_cautious_bakers;
  int seed;

  /* Process command-line arguments */
  if (argc != 5) {
    fprintf(stderr,"Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n",argv[0]);
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
    exit(1);
  }
 
  /* Seed the RNG */
  srand(seed);

  // IMPLEMENT CODE HERE

  exit(0);
}


