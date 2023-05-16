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
	
	 // even processors
	if (my_rank%2 == 0) {
		if (my_rank == 0) 
			sprintf(message, "ring1");
		if (my_rank == 0) { // first
			cout << my_rank << " has ring 1" << endl;
			// if p = 10
			// if p = 9
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, 2, tag, MPI_COMM_WORLD); // 0 sends to 2; 0 sends to 2
			if (p%2 == 0)
				MPI_Recv(message, 100, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD, &status); // 0 receives from 8
			else
				MPI_Recv(message, 100, MPI_CHAR, p - 1, tag, MPI_COMM_WORLD, &status); // 0 receives from 8
			cout << "Provisional victory against no one!" << endl;
		}
		else if (my_rank == p - 2) { // last
			MPI_Recv(message, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status); // 8 receives from 6
			cout << my_rank << " has ring 1" << endl;
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD); // 8 sends to 0
		}
		else { // middle
			MPI_Recv(message, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
			cout << my_rank << " has ring 1" << endl;
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD);
		}
	}
	// odd processes
	else {
		if (my_rank == 1) 
			sprintf(message, "ring2");
		if (my_rank == 1) { // first
			cout << my_rank << " has ring 2" << endl;
			// if p = 10
			// if p = 9
			if (p%2 == 0) {
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, p-1, tag, MPI_COMM_WORLD); // 1 sends to 9
				MPI_Recv(message, 100, MPI_CHAR, p+2, tag, MPI_COMM_WORLD, &status); // 1 receives from 3
			}
			else {
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, p-2, tag, MPI_COMM_WORLD); // 1 sends to 7
				MPI_Recv(message, 100, MPI_CHAR, p+2, tag, MPI_COMM_WORLD, &status); // 1 recieves from 3
			}
			cout << "Provisional victory against no one!" << endl;
		}
		else if (my_rank == p - 1) { // last
			MPI_Recv(message, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
			cout << my_rank << " has ring 2" << endl;
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
		}
		else { // middle
			MPI_Recv(message, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
			cout << my_rank << " has ring 2" << endl;
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD);
		}
	}

	// Shut down MPI
	MPI_Finalize();

	return 0;
}