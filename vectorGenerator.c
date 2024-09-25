/* Vector generator - simple code to generate 20 vectors of random values from 0 to 99, 30 max values per vector.*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_VECTORS 20
#define MAX_VALUES 30
#define MAX_RANDOM 100

void generate_vectors(int vectors[NUM_VECTORS][MAX_VALUES]) {
    srand(time(NULL));
    for (int i = 0; i < NUM_VECTORS; i++) {
        for (int j = 0; j < MAX_VALUES; j++) {
            vectors[i][j] = rand() % MAX_RANDOM;
        }
    }
}

void print_vectors(int vectors[NUM_VECTORS][MAX_VALUES]) {
    for (int i = 0; i < NUM_VECTORS; i++) {
        printf("Vector %d: ", i + 1);
        for (int j = 0; j < MAX_VALUES; j++) {
            printf("%d ", vectors[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int vectors[NUM_VECTORS][MAX_VALUES];
    generate_vectors(vectors);
    print_vectors(vectors);
    return 0;
}