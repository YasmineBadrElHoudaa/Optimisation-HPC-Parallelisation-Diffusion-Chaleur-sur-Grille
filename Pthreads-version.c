#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define SIZE 1000       // Taille de la grille
#define ITERATIONS 1000 // Nombre d'itérations
#define NUM_THREADS 4   // Nombre de threads

double** grid;
double** new_grid;

// Structure pour passer des paramètres aux threads
typedef struct {
    int start_row;
    int end_row;
} thread_data_t;

// Fonction exécutée par chaque thread
void* simulate_heat_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    for (int t = 0; t < ITERATIONS; t++) {
        for (int i = data->start_row; i < data->end_row; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                new_grid[i][j] = 0.25 * (grid[i-1][j] + grid[i+1][j] + grid[i][j-1] + grid[i][j+1]) +
                                  10 * sin(i * 0.01) * cos(j * 0.01);
            }
        }
    }
    pthread_exit(NULL);
}

int main() {
    grid = (double**)malloc(SIZE * sizeof(double*));
    new_grid = (double**)malloc(SIZE * sizeof(double*));
    for (int i = 0; i < SIZE; i++) {
        grid[i] = (double*)malloc(SIZE * sizeof(double));
        new_grid[i] = (double*)malloc(SIZE * sizeof(double));
    }

    // Initialisation aléatoire
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = rand() % 100;

    clock_t start = clock();

    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    // Lancement des threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_row = i * (SIZE / NUM_THREADS);
        thread_data[i].end_row = (i + 1) * (SIZE / NUM_THREADS);
        if (i == NUM_THREADS - 1) {
            thread_data[i].end_row = SIZE; // Assurer que le dernier thread couvre jusqu'à la fin
        }
        pthread_create(&threads[i], NULL, simulate_heat_thread, (void*)&thread_data[i]);
    }

    // Attendre que tous les threads terminent
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Mettre à jour la grille après que tous les threads aient terminé
    for (int i = 1; i < SIZE - 1; i++) {
        for (int j = 1; j < SIZE - 1; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }

    clock_t end = clock();

    printf("Temps d'exécution parallèle : %lf secondes\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < SIZE; i++) {
        free(grid[i]);
        free(new_grid[i]);
    }
    free(grid);
    free(new_grid);

    return 0;
}
