#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 21
#define THREADS 4

typedef struct {
    int start;
    int end;
    int *arr;
} thread_data_t;

pthread_barrier_t barrier;

int data[21] = {5, 8, 3, 4, 1, 3, 2, 7, 6, 10, 9, 1, 3, 8, 3, 9, 4, 2, 11, 6, 5};
int pdat[21] = {5, 8, 3, 4, 1, 3, 2, 7, 6, 10, 9, 1, 3, 8, 3, 9, 4, 2, 11, 6, 5};
int test[9] = {8, 4, 1, 10, 2, 7, 6, 9, 3};
int ptst[9] = {8, 4, 1, 10, 2, 7, 6, 9, 3};

void print(int *p, int size) {
    int i = 0;
    for (; i < size; i++) {
        printf("%d ", p[i]);
    }
    printf("\n");
}

void swap(int *p, int i, int j) {
    int temp = p[i];
    p[i] = p[j];
    p[j] = temp;
}

void serial_odd_even_sort(int *p, int size) {
    int swapped, i;

    do {
        swapped = 0;

        for (i = 1; i < size-1; i+=2) { //odd
            if (p[i] > p[i+1]) {
                swap(p, i, i+1);
                swapped = 1;
            }
        }
        printf("odd : ");
        print(p, SIZE);
        for (i = 0; i < size-1; i+=2) { //even
            if (p[i] > p[i+1]) {
                swap(p, i, i+1);
                swapped = 1;
            }
        }
        printf("even: ");
        print(p, SIZE);

    } while (swapped);
}

void *thread(void *arg) {
    int start = ((thread_data_t *) arg)->start;
    int end = ((thread_data_t *) arg)->end;
    int *arr = ((thread_data_t *) arg)->arr;

    int swapped, i;
    do {
        swapped = 0;

        for (i = start + 1; i < end - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }
        print(arr, SIZE);
        pthread_barrier_wait(&barrier);

        for (i = start; i < end - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }
        print(arr, SIZE);
        pthread_barrier_wait(&barrier);
        printf("sw = %d thr = %d\n", swapped, start);
    } while (swapped);
    printf("exit %d\n", start);
    pthread_exit(NULL);
}

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
        printf("thr = %d, s = %d, e = %d\n", i, thread_data->start, thread_data->end);
        assert(!pthread_create(&threads[i], NULL, thread, (void *) thread_data));
    }

    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
//    serial_odd_even_sort(test, SIZE);
    parallel_odd_even_sort(THREADS, data, SIZE);

    return 0;
}



