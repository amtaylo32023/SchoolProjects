#include <iostream>
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
	int n = 1000000;
	srand(71911);
	int * a = new int[n];
	int max = 0;
	int min = 0;
	int avg = 0;
	
	if (my_rank == 0) {
		for (int x = 0; x < n; x++)
			a[x] = rand() % 8;
	}

	// 2. break up the problem
	int localn = n/p;
	int * locala = new int[localn];
	MPI_Scatter(&a[0], localn, MPI_INT, locala, localn, MPI_INT, 0, MPI_COMM_WORLD);
	
	// 3a. do local max
	int local_max = locala[0];
	
	for (int x = 0; x < localn; x++) {
		if (locala[x] > local_max)
			local_max = locala[x];
	}
	cout << my_rank << " local max is " << local_max << endl;
	
	// 4a. combine max back together
	MPI_Allreduce(&local_max, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	
	if (my_rank == 0)
		cout << "Maximum: " << max << endl;
	
	// 3b. do local min
	int local_min = locala[0];
	
	for (int x = 0; x < localn; x++) {
		if (locala[x] < local_min)
			local_min = locala[x];
	}
	cout << my_rank << " local min is " << local_min << endl;
	
	// 4b. combine min back together
	MPI_Allreduce(&local_min, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	
	if (my_rank == 0)
		cout << "Minimum: " << min << endl;
	
	// 3c. do local average
	int local_sum = 0;
	
	for (int x = 0; x < localn; x++) 
		local_sum += locala[x];
	
	int local_avg = local_sum / localn;
	cout << my_rank << " : " << local_avg << " : " << local_sum << endl;
	
	// 4c. combine average back together
	MPI_Allreduce(&local_avg, &avg, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	int realavg = avg / p;
	
	if (my_rank == 0)
		cout << "Average " << realavg <<  endl;
	
	// Shutdown Procedures
	delete [] a;
	delete [] locala;

	// Shut down MPI
	MPI_Finalize();

	return 0;
}