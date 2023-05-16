#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "mpi.h" // message passing interface
using namespace std;

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

int main (int argc, char * argv[]) {

	int my_rank;			// my CPU number for this process
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number
	char message[100];		// message itself
	MPI_Status status;		// return status for receive
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	// THE REAL PROGRAM IS HERE
	// 1. set up the problem
    // this text file has 2695 words (one each line all lowercase) which I created from a fun hangman program I wrote last summer
	int n = 16652;
	char book[n];
	int counts[26] = {0};
	int i = 0;
	ifstream file("WordsList.txt");
	if(file.is_open()) {
		while(!file.eof()) {
			file >> book[i];
			i++;
		}
	}
	
	// 2. break up the problem
	int localn = n/p;
	char localbook[n];
	int localcounts[26] = {0};
	MPI_Scatter(&book, localn, MPI_CHAR, &localbook, localn, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	// 3. do the local work
	for(int x = 0; x < localn; x++)
		localcounts[(int)localbook[x] - 97]++;
	
	// 4. bring back together
	MPI_Reduce(&localcounts, &counts, 26, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);\
	
	if (my_rank == 0) {
		for (int x = 0; x < 26; x++) 
			cout << (char)(97 + x) << " " << counts[x] << endl;
	}
	
	// Shut down MPI
	MPI_Finalize();

	return 0;
}