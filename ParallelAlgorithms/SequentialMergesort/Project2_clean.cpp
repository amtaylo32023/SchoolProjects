#include <iostream>
using namespace std;

void mergesort (int * a, int first, int last);
void merge(int * a, int * b, int lasta, int lastb, int * output = NULL);

int main (int argc, char * argv[]) {
	int arraysize;
	cout << "Array size: ";
	cin >> arraysize; // user input size
	int n = 1000000;
	srand(71911);
	int * a = new int[arraysize];
	
	cout << endl << "Original array: " << endl;
	for (int x = 0; x < arraysize; x++) { // fill a with random numbers
		a[x] = rand() % n;
		cout << a[x] << endl;
	}
		
	mergesort(a, 0, arraysize); // call mergesort
	
	cout << endl << "Sorted array: " << endl;
	for (int x = 0; x < arraysize; x++) // print sorted array
		cout << a[x] << endl;
}

void mergesort (int * a, int first, int last) {
	if (last == 1) // split to base
		return;
	int mid = last / 2; // calculate mid
	int *b = &a[mid]; // assign b to start at a[mid]
	mergesort(a, first, mid); // first half
	mergesort(b, first, last - mid); // second half
	merge(a, b, mid, last - mid, NULL);	// stick back together
}

void merge(int * a, int * b, int lasta, int lastb, int * output) {
	if (output == NULL) // output isn't made yet
		output = new int[lasta + lastb];
	
	for (int i = 0; i < lasta; i++) { // go through a array
		for (int j = 0; j < lastb; j++) { // go through b array
			for (int k = 0; k < (lasta + lastb); k++) { // go through output array
				while(i < lasta && j < lastb) { // while a and b still have stuff in them
					if (a[i] < b[j]) // if a is bigger
						output[k++] = a[i++];
					else // if b is bigger or same as a
						output[k++] = b[j++];
				}
				
				while (i < lasta) // while a has leftover stuff
					output[k++] = a[i++];
				
				while (j < lastb) // while b has leftover stuff
					output[k++] = b[j++];
			}
		}
	}

	for (int i = 0; i < lasta + lastb; i++) // make a equal to output array
		a[i] = output[i]; 
}