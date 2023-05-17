#include <stdio.h>
#include <string.h>

#include <cmath>
#include <iostream>

#include "mpi.h"  // message passing interface
#define MAX_RAND 500

using namespace std;

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

// global variables
int my_rank;  // my CPU number for this process
int p;        // number of CPUs that we have

void printArray(int* a, int size) {
    for (int i = 0; i < size; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}

// source: https://www.coderslexicon.com/generating-non-repeating-random-values-in-c/
void shuffle(int* values, int size) {
    // Seed our random number generator.
    srand(time(0));

    // Create large number of swaps
    // size * 20 for the number of swaps
    for (int i = 0; i < (size * 20); i++) {
        // Generate random values for subscripts, not values!
        int randvalue1 = (rand() % size) + 0;
        int randvalue2 = (rand() % size) + 0;

        int temp = values[randvalue1];
        values[randvalue1] = values[randvalue2];
        values[randvalue2] = temp;
    }
}

int mylog2(int val) {
    int counter = 0;
    while (val != 1) {
        counter++;
        val /= 2;
    }
    return counter;
}

// https://stackoverflow.com/a/2745086
int ceil_x_div_y(int x, int y) {
    return (x + y - 1) / y;
}

int myrank(int* a, int first, int last, int val) {
    // ceil(middle)
    int middle = ceil_x_div_y(first + last, 2);
    // base case
    if (last - first == 0) {
        if (val < a[first]) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (val < a[middle]) {
            return myrank(a, first, middle - 1, val);
        } else {
            return middle + myrank(a + middle, 0, last - middle, val);
        }
    }
    return -1;
}

void smerge(int* a, int* b, int lasta, int lastb, int* output) {
    int size = lasta + lastb;
    int* combined = new int[size];

    int i = 0;
    int j = 0;
    int k = 0;

    while (i < lasta && j < lastb) {  // while a and b still have stuff in them
        if (a[i] < b[j])              // if a is bigger
            combined[k++] = a[i++];
        else  // if b is bigger or same as a
            combined[k++] = b[j++];
    }

    while (i < lasta)  // while a has leftover stuff
        combined[k++] = a[i++];

    while (j < lastb)  // while b has leftover stuff
        combined[k++] = b[j++];

    // if an output array is provided, load the results into it
    if (output) {
        std::copy(combined, combined + size, output);
    }

    delete[] combined;
}

void smergesort(int* a, int first, int last) {
    if (last == 1)  // split to base
        return;
    int mid = last / 2;                // calculate mid
    int* b = a + mid;                  // assign b to start at a[mid]
    smergesort(a, first, mid);         // first half
    smergesort(b, mid, last - mid);    // second half
    smerge(a, b, mid, last - mid, a);  // stick back together
}

bool isSelect(int val, int size, int sample_size) {
    int divisor = mylog2(size / 2);
    // case where size = 2, wherein the 1 element in each array is guaranteed to have been sampled
    if (divisor == 0) {
        return true;
    }
    // value must not be greater than logn * samplesize and must be perfectly divisible by logn
    return (val / divisor) + 1 <= sample_size && !(val % divisor);
}

void pmerge(int* a, int* b, int lasta, int lastb, int* output = NULL) {
    int size = lasta + lastb;
    int logn = mylog2(size / 2);
    int* L_WIN = new int[size]();
    int* WIN = new int[size]();
    int index = 0;

    int sample_size = 0;
    if (size == 2) {
        // hard code sampling to 1 element when only one item in each list
        sample_size = 1;
    } else {
        sample_size = (size / 2) / logn;
    }
    int* SAMPLEA = new int[sample_size]();
    int* SAMPLEB = new int[sample_size]();

    // Sample and calculate ranks from a and b
    for (int i = my_rank; i < sample_size; i += p) {
        index = i * logn;
        SAMPLEA[i] = myrank(b, 0, (size / 2) - 1, a[index]);
        SAMPLEB[i] = myrank(a, 0, (size / 2) - 1, b[index]);
    }

    int* SRANKA = new int[sample_size]();
    int* SRANKB = new int[sample_size]();

    MPI_Allreduce(SAMPLEA, SRANKA, sample_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(SAMPLEB, SRANKB, sample_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // place sampled items in to WIN
    for (int i = my_rank; i < sample_size; i += p) {
        index = i * logn;
        L_WIN[index + SRANKA[i]] = a[index];
        L_WIN[index + SRANKB[i]] = b[index];
    }

    int* L_ENDPA = new int[sample_size * 2]();
    int* L_ENDPB = new int[sample_size * 2]();

    // +1 to hold auxiliary element used for shape processing
    int* ENDPA = new int[(sample_size * 2) + 1]();
    int* ENDPB = new int[(sample_size * 2) + 1]();

    // populate endpoint arrays
    for (int i = my_rank; i < sample_size; i += p) {
        L_ENDPA[i] = SRANKB[i];
        L_ENDPB[i] = SRANKA[i];
    }
    for (int i = my_rank + sample_size; i < sample_size * 2; i += p) {
        index = (i - sample_size) * logn;
        L_ENDPA[i] = index;
        L_ENDPB[i] = index;
    }

    MPI_Allreduce(L_ENDPA, ENDPA, sample_size * 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(L_ENDPB, ENDPB, sample_size * 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if (my_rank == 0) {
        // sort to ensure endpoints are aligned correctly
        smerge(ENDPA, ENDPA + sample_size, sample_size, sample_size, ENDPA);
        smerge(ENDPB, ENDPB + sample_size, sample_size, sample_size, ENDPB);
    }

    MPI_Bcast(ENDPA, sample_size * 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(ENDPB, sample_size * 2, MPI_INT, 0, MPI_COMM_WORLD);

    // auxillary element at the end of endpoint arrays
    ENDPA[(sample_size * 2)] = size / 2;
    ENDPB[(sample_size * 2)] = size / 2;
    int starta = 0, startb = 0;
    int nexta = 0, nextb = 0;
    int select_offseta = 0, select_offsetb = 0;
    int start = 0, count = 0;
    int offseta = 0, offsetb = 0, woffset = 0;

    for (int i = my_rank; i < sample_size * 2; i += p) {
        // grab endpoints of current shape
        starta = ENDPA[i];
        nexta = ENDPA[i + 1];
        startb = ENDPB[i];
        nextb = ENDPB[i + 1];
        // determine if top endpoints are elements that were previously sampled
        select_offseta = isSelect(starta, size, sample_size);
        select_offsetb = isSelect(startb, size, sample_size);

        if (starta == nexta) {  // shape contains no elements from A
            start = startb + select_offsetb;
            count = nextb - start;
            woffset = starta + start;
            // place shape in WIN
            std::copy(b + start, b + start + count, L_WIN + woffset);
        } else if (startb == nextb) {  // shape contains no elements from B
            start = starta + select_offseta;
            count = nexta - start;
            woffset = start + startb;
            // place shape in WIN
            std::copy(a + start, a + start + count, L_WIN + woffset);
        } else {  // complex shape, merging needed
            offseta = starta + select_offseta;
            offsetb = startb + select_offsetb;
            woffset = starta + select_offseta + startb + select_offsetb;
            smerge(a + offseta, b + offsetb, nexta - offseta, nextb - offsetb, L_WIN + woffset);
        }
    }

    MPI_Allreduce(L_WIN, WIN, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // if an output array is provided, load the results into it
    if (output) {
        std::copy(WIN, WIN + size, output);
    }

    // memory management
    delete[] SAMPLEA;
    delete[] SAMPLEB;
    delete[] SRANKA;
    delete[] SRANKB;
    delete[] L_ENDPA;
    delete[] L_ENDPB;
    delete[] ENDPA;
    delete[] ENDPB;
    delete[] L_WIN;
    delete[] WIN;
}

void pmergesort(int* a, int first, int last) {
    if (last == 1)  // split to base
        return;
    int mid = last / 2;                // calculate mid
    int* b = a + mid;                  // assign b to start at a[mid]
    pmergesort(a, first, mid);         // first half
    pmergesort(b, mid, last - mid);    // second half
    pmerge(a, b, mid, last - mid, a);  // stick back together
}

int main(int argc, char* argv[]) {
    // Start MPI
    MPI_Init(&argc, &argv);

    // Find out my rank!
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Find out the number of processes!
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // THE REAL PROGRAM IS HERE
    int size = 0;
    int* a = NULL;

    if (my_rank == 0) {
        // fill range array (ensure non repeating random values)
        int range[MAX_RAND + 1];
        for (int i = 0; i < MAX_RAND + 1; i++) {
            range[i] = i;
        }

        cout << "Enter size: ";
        cin >> size;
        // check for power of 2 and >0
        if (size < 1 || size > 128 || (size & (size - 1)) != 0) {
            cout << "size invalid" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // random shuffle of range array
        shuffle(range, MAX_RAND + 1);
        a = new int[size];

        // grab first size values of range as input
        std::copy(range, range + size, a);
    }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {
        a = new int[size];
    }

    MPI_Bcast(a, size, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printArray(a, size);
        cout << endl;
    }

    pmergesort(a, 0, size);

    if (my_rank == 0) {
        printArray(a, size);
    }

    // Shut down MPI
    MPI_Finalize();

    return 0;
}