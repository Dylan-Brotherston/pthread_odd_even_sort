/* Serial implementation of the odd-even sort
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>

// Somethings for debugging
#define DEBUG 0
#if DEBUG
#define ASSERT
#else
#define SHOW
#endif

#define MAX 1000
#define ITR 1000

#ifdef SHOW
#define PRINT printf
#else
#define PRINT(...)
#endif

#ifdef ASSERT
#define CHECK assert
#else
#define CHECK(...)
#endif

/* generate some random data of given size
 */
int *generate_data(int size) {
	int *p = malloc(sizeof(int) * size);
	int i;
	for(i = 0; i < size; i++) {
		p[i] = rand() % MAX;
	}

	return p;
}

/* display the data and/or assert
 */
void show(int p[], int size) {
	int i;
#ifdef SHOW
	PRINT("{");
	for(i = 0; i < size; i++) {
		PRINT("%d ", p[i]);
	}
#endif

#ifdef ASSERT
	for(i = 1; i < size; i++) {
		CHECK(p[i-1] <= p[i]);
	}
#endif
}

/* swap.
 * @ p: ref. to array
 * @ i: first arg
 * @ j: second arg
 */
void swap(int *p, int i, int j) {
	int tmp = p[i];
	p[i] = p[j];
	p[j] = tmp;
}

/* parallel odd-even-sort.
 * @ threads: how many threads to create
 * @ p: pointer to the data set
 * @ size: size of the data set
 */
void parallel_odd_even_sort(int threads, int *p, int size) {
	return;
}

/* serial odd-even-sort
 * @ p: pointer to the data set
 * @ size: size of the data set
 */
void serial_odd_even_sort(int *p, int size) {
	int swapped, i;

	do {
		swapped = 0;

		for(i = 1; i < size-1; i += 2) { //odd
			if(p[i] > p[i+1]) {
			   swap(p, i, i+1);
			   swapped = 1;
			}
		}

		for(i = 0; i < size-1; i += 2) { //even
			if(p[i] > p[i+1]) {
			   swap(p, i, i+1);
			   swapped = 1;
			}
		}

	} while(swapped);
}

int main() {
	int i, thr;
	struct timespec start, end;

	/* Sit in a loop.
	* Create a data set of different size,
	* Call serial version and measure the time
	* Call the parallel version with different number of threads
	* 4 to 8 addition two additional threads each time.
	*/
	for(i = 100; i < ITR; i += 100) {
		int *p = generate_data(i);

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		{
			serial_odd_even_sort(p, i);
		}
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		//PRINT("size = %d\tdelta_us = %lu ", i, delta_us);
		show(p, i);
		/*
		for(thr = 4; thr < 9; thr += 2) {
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			{
				parallel_odd_even_sort(thr, p, i);
			}
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);

			uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
			PRINT("%lu ", delta_us);
			show(p, i);
		}*/

		PRINT("\n");
		free(p);
	}
}

// Can we use the same p?
// should we generate a different p?
// should be duplication p?


