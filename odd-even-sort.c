/* Serial implementation of the odd-even sort
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

// Somethings for debugging
#define DEBUG 1
#if DEBUG
#define ASSERT
#else
#define SHOW
#endif

#define MAX 1000
#define ITR 10000

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

typedef struct {
    int start;
    int end;
    int *arr;
} thread_data_t;

static int g_swapped;
pthread_barrier_t barrier;
pthread_mutex_t mutex;


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

void *thread(void *arg) {
    int start = ((thread_data_t *) arg)->start;
    int end = ((thread_data_t *) arg)->end;
    int *arr = ((thread_data_t *) arg)->arr;

    int swapped, i;
    do {
        pthread_barrier_wait(&barrier);
        g_swapped = 0;
        swapped = 0;

        for (i = start + 1; i < end - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }

        pthread_barrier_wait(&barrier);

        for (i = start; i < end - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }

        pthread_mutex_lock(&mutex);
        g_swapped = swapped | g_swapped;
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);

    } while (g_swapped);

    pthread_exit(NULL);
}

/* parallel odd-even-sort.
 * @ threads: how many threads to create
 * @ p: pointer to the data set
 * @ size: size of the data set
 */
void parallel_odd_even_sort(int thread_count, int *p, int size) {
    int i, frac = size/thread_count;
    pthread_barrier_init(&barrier, NULL, thread_count);
    pthread_t threads[thread_count];

    for (i = 0; i < thread_count; i++) {
        thread_data_t *thread_data = (thread_data_t *) malloc(sizeof(thread_data_t));
        int iintof = i * frac;
        if (iintof % 2 == 1) {
            thread_data->start = iintof - 1;
        } else {
            thread_data->start = iintof;
        }
        thread_data->end = iintof + frac + 1;
        thread_data->arr = p;
        assert(!pthread_create(&threads[i], NULL, thread, (void *) thread_data));
    }

    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
}


int main() {
    FILE *f = fopen("data.csv", "w");
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
		PRINT("%d,%d,%lu\n", 1, i, delta_us);
		fprintf(f, "%d,%d,%lu\n", 1, i, delta_us);
//		show(p, i);

		for(thr = 2; thr < 9; thr += 2) {
			int size = sizeof(int) * i;
			int *pp = malloc(size);
			memcpy(pp, p, size);
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			{
				parallel_odd_even_sort(thr, p, i);
			}
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);

			uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
			PRINT("%d,%d,%lu\n", thr, i, delta_us);
			fprintf(f, "%d,%d,%lu\n", thr, i, delta_us);
//			show(p, i);
		}

//		PRINT("\n");
		free(p);
	}
}

// Can we use the same p?
// should we generate a different p?
// should be duplication p?


