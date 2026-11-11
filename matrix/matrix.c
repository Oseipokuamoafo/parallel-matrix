#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAXN 20        // largest matrix size the static arrays can hold
#define NUM_THREADS 10 // total threads used per operation

int MAX = 20;          // actual matrix size (can be overridden from the command line)

int matA[MAXN][MAXN];
int matB[MAXN][MAXN];

int matSumResult[MAXN][MAXN];
int matDiffResult[MAXN][MAXN];
int matProductResult[MAXN][MAXN];

void fillMatrix(int matrix[MAXN][MAXN]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAXN][MAXN]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Each thread receives its thread number i (malloc'd int*) and handles
// rows i, i + NUM_THREADS, i + 2*NUM_THREADS, ... of the result matrix.

void* computeSum(void* args) {
    int tid = *(int*)args;
    free(args);
    for (int i = tid; i < MAX; i += NUM_THREADS)
        for (int j = 0; j < MAX; j++)
            matSumResult[i][j] = matA[i][j] + matB[i][j];
    return NULL;
}

void* computeDiff(void* args) {
    int tid = *(int*)args;
    free(args);
    for (int i = tid; i < MAX; i += NUM_THREADS)
        for (int j = 0; j < MAX; j++)
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
    return NULL;
}

void* computeProduct(void* args) {
    int tid = *(int*)args;
    free(args);
    for (int i = tid; i < MAX; i += NUM_THREADS) {
        for (int j = 0; j < MAX; j++) {
            int sum = 0;
            for (int k = 0; k < MAX; k++)
                sum += matA[i][k] * matB[k][j];
            matProductResult[i][j] = sum;
        }
    }
    return NULL;
}

// Spawns NUM_THREADS (10) threads per operation; each handles a share of the rows.
int main(int argc, char* argv[]) {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.

    // 0. Get the matrix size from the command line and assign it to MAX
    if (argc > 1) {
        int n = atoi(argv[1]);
        if (n > 0 && n <= MAXN)
            MAX = n;
    }

    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread_t objects for our threads.
    pthread_t sumThreads[NUM_THREADS];
    pthread_t diffThreads[NUM_THREADS];
    pthread_t prodThreads[NUM_THREADS];

    // 4. Create threads for each matrix operation, passing each its thread number.
    for (int i = 0; i < NUM_THREADS; i++) {
        int* a = malloc(sizeof(int)); *a = i;
        pthread_create(&sumThreads[i], NULL, computeSum, a);
        int* b = malloc(sizeof(int)); *b = i;
        pthread_create(&diffThreads[i], NULL, computeDiff, b);
        int* c = malloc(sizeof(int)); *c = i;
        pthread_create(&prodThreads[i], NULL, computeProduct, c);
    }

    // 5. Wait for all threads to finish.
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(sumThreads[i], NULL);
        pthread_join(diffThreads[i], NULL);
        pthread_join(prodThreads[i], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
}