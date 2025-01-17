#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 1000       // Taille de la grille
#define ITERATIONS 1000 // Nombre d'itérations

void simulate_heat(double** grid) {
    double** new_grid = (double**)malloc(SIZE * sizeof(double*));
    for (int i = 0; i < SIZE; i++) {
        new_grid[i] = (double*)malloc(SIZE * sizeof(double));
    }

    for (int t = 0; t < ITERATIONS; t++) {
        for (int i = 1; i < SIZE - 1; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                new_grid[i][j] = 0.25 * (grid[i-1][j] + grid[i+1][j] + grid[i][j-1] + grid[i][j+1]) +
                                  10 * sin(i * 0.01) * cos(j * 0.01);
            }
        }
        // Mettre à jour la grille
        for (int i = 1; i < SIZE - 1; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                grid[i][j] = new_grid[i][j];
            }
        }
    }

    for (int i = 0; i < SIZE; i++) {
        free(new_grid[i]);
    }
    free(new_grid);
}

int main() {
    double** grid = (double**)malloc(SIZE * sizeof(double*));
    for (int i = 0; i < SIZE; i++) {
        grid[i] = (double*)malloc(SIZE * sizeof(double));
    }

    // Initialisation aléatoire
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = rand() % 100;

    clock_t start = clock();
    simulate_heat(grid);
    clock_t end = clock();

    printf("Temps d'exécution séquentiel : %lf secondes\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < SIZE; i++) {
        free(grid[i]);
    }
    free(grid);

    return 0;
}
