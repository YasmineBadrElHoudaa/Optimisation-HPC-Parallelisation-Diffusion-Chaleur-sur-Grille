%%cuda
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TAILLE 100
#define ITERATIONS 100
#define N 16

__global__ void calculer_nouvelle_grille(double* grille, double* nouvelle_grille, int taille) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;

    if (i > 0 && i < taille - 1 && j > 0 && j < taille - 1) {
        nouvelle_grille[i * taille + j] = 0.25 * (grille[(i - 1) * taille + j] + grille[(i + 1) * taille + j] +
                                                 grille[i * taille + (j - 1)] + grille[i * taille + (j + 1)]) +
                                         10 * sin(i * 0.01) * cos(j * 0.01);
    }
}

__global__ void mettre_a_jour_grille(double* grille, double* nouvelle_grille, int taille) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;

    if (i > 0 && i < taille - 1 && j > 0 && j < taille - 1) {
        grille[i * taille + j] = nouvelle_grille[i * taille + j];
    }
}

int main() {
    double* h_grille = (double*)malloc(TAILLE * TAILLE * sizeof(double));
    double* h_nouvelle_grille = (double*)malloc(TAILLE * TAILLE * sizeof(double));

    srand(time(NULL));
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            h_grille[i * TAILLE + j] = rand() % 100;
        }
    }

    double *d_grille, *d_nouvelle_grille;
    cudaMalloc((void**)&d_grille, TAILLE * TAILLE * sizeof(double));
    cudaMalloc((void**)&d_nouvelle_grille, TAILLE * TAILLE * sizeof(double));

    cudaMemcpy(d_grille, h_grille, TAILLE * TAILLE * sizeof(double), cudaMemcpyHostToDevice);
      // N*1 = nombre de threads par Bloc
    dim3 threadsParBloc(N, 1);
   
    dim3 blocsParGrille((TAILLE + threadsParBloc.x - 1) / threadsParBloc.x,
                        (TAILLE + threadsParBloc.y - 1) / threadsParBloc.y);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    for (int t = 0; t < ITERATIONS; t++) {
        calculer_nouvelle_grille<<<blocsParGrille, threadsParBloc>>>(d_grille, d_nouvelle_grille, TAILLE);
        cudaDeviceSynchronize();

        mettre_a_jour_grille<<<blocsParGrille, threadsParBloc>>>(d_grille, d_nouvelle_grille, TAILLE);
        cudaDeviceSynchronize();

        if (t % 100 == 0) {
            printf("Iteration %d/%d terminee\n", t, ITERATIONS);
        }
    }

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Temps d'execution: %.2f secondes\n", milliseconds / 1000.0);

    cudaMemcpy(h_grille, d_grille, TAILLE * TAILLE * sizeof(double), cudaMemcpyDeviceToHost);

    cudaFree(d_grille);
    cudaFree(d_nouvelle_grille);

    free(h_grille);
    free(h_nouvelle_grille);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}
