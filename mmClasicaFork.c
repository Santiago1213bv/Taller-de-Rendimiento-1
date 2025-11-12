/*
 * Pontificia Universidad Javeriana — Taller de Evaluación de Rendimiento
 * Autor original: J. Corredor, PhD
 * Documentado por: Santiago Bautista Velásquez
 * Fecha: 2025-11-10
 *
 * Descripción general:
 * ---------------------------------------------------------------
 * Implementación del algoritmo clásico de multiplicación de matrices,
 * utilizando paralelismo a nivel de procesos mediante la llamada `fork()`.
 * 
 * Cada proceso hijo calcula un subconjunto de filas de la matriz resultado,
 * aprovechando múltiples núcleos del CPU para dividir la carga de trabajo.
 *
 * Estructura general:
 *  - Función `iniMatrix()`: inicializa las matrices A y B con valores aleatorios.
 *  - Función `multiMatrix()`: realiza la multiplicación parcial por bloques de filas.
 *  - Función `impMatrix()`: imprime una matriz (solo si es pequeña, N < 9).
 *  - Funciones `InicioMuestra()` y `FinMuestra()`: miden el tiempo de ejecución total.
 *  - `main()`: distribuye el trabajo entre procesos hijos, espera su finalización
 *    y libera memoria al concluir.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

struct timeval inicio, fin;

/*-----------------------------------------------------------------------------
 * InicioMuestra — Inicia el cronómetro de medición de tiempo.
 *
 * Descripción:
 *  Guarda la marca de tiempo actual en la variable global `inicio`
 *  mediante la función `gettimeofday()`.
 *---------------------------------------------------------------------------*/
void InicioMuestra() {
	gettimeofday(&inicio, (void *)0);
}

/*-----------------------------------------------------------------------------
 * FinMuestra — Finaliza la medición y muestra el tiempo total.
 *
 * Descripción:
 *  Calcula el tiempo transcurrido desde `InicioMuestra()` en microsegundos
 *  y lo imprime con formato numérico.
 *---------------------------------------------------------------------------*/
void FinMuestra() {
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec * 1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

/*-----------------------------------------------------------------------------
 * multiMatrix — Multiplicación parcial de matrices.
 *
 * Parámetros:
 *  - mA: puntero a la matriz A (en formato lineal)
 *  - mB: puntero a la matriz B (en formato lineal)
 *  - mC: puntero a la matriz resultado C
 *  - D:  dimensión de las matrices (D x D)
 *  - filaI: índice de fila inicial a procesar
 *  - filaF: índice de fila final a procesar
 *
 * Descripción:
 *  Cada proceso hijo ejecuta esta función sobre un subconjunto de filas.
 *  Calcula los productos parciales y los almacena directamente en `mC`.
 *---------------------------------------------------------------------------*/
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF) {
	double Suma, *pA, *pB;

	for (int i = filaI; i < filaF; i++) {
		for (int j = 0; j < D; j++) {
			Suma = 0.0;
			pA = mA + i * D;
			pB = mB + j;

			for (int k = 0; k < D; k++, pA++, pB += D) {
				Suma += *pA * *pB;	
			}
			mC[i * D + j] = Suma;
		}
	}
}

/*-----------------------------------------------------------------------------
 * impMatrix — Imprime una matriz cuadrada de tamaño D×D.
 *
 * Parámetros:
 *  - matrix: puntero a la matriz en formato lineal.
 *  - D: tamaño de la matriz (dimensión).
 *
 * Descripción:
 *  Si la matriz es pequeña (D < 9), se imprime en pantalla con formato.
 *  Se usa principalmente con fines de depuración.
 *---------------------------------------------------------------------------*/
void impMatrix(double *matrix, int D) {
	if (D < 9) {
		printf("\nImpresión de matriz:\n");
		for (int i = 0; i < D * D; i++, matrix++) {
			if (i % D == 0) printf("\n");
			printf(" %.2f ", *matrix);
		}
		printf("\n ");
	}
}

/*-----------------------------------------------------------------------------
 * iniMatrix — Inicializa las matrices A y B con valores aleatorios.
 *
 * Parámetros:
 *  - mA: puntero a la matriz A.
 *  - mB: puntero a la matriz B.
 *  - D:  tamaño de las matrices cuadradas.
 *
 * Descripción:
 *  Asigna valores aleatorios de punto flotante a las dos matrices:
 *   - A: valores entre 1.0 y 5.0
 *   - B: valores entre 5.0 y 9.0
 *---------------------------------------------------------------------------*/
void iniMatrix(double *mA, double *mB, int D) {
	for (int i = 0; i < D * D; i++, mA++, mB++) {
		*mA = (double) rand() / RAND_MAX * (5.0 - 1.0);
		*mB = (double) rand() / RAND_MAX * (9.0 - 5.0);
	}
}

/*-----------------------------------------------------------------------------
 * main — Función principal del programa.
 *
 * Parámetros:
 *  - argc: número de argumentos pasados en la línea de comandos.
 *  - argv: arreglo de argumentos (argv[1] = tamaño, argv[2] = procesos).
 *
 * Descripción:
 *  1. Valida los parámetros de entrada.
 *  2. Reserva memoria dinámica para matrices A, B y C.
 *  3. Inicializa y muestra las matrices (si son pequeñas).
 *  4. Divide el trabajo entre procesos hijos usando `fork()`.
 *  5. Cada hijo calcula un rango de filas de la matriz C.
 *  6. El proceso padre espera la finalización de todos los hijos.
 *  7. Mide y muestra el tiempo total de ejecución.
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("\nUso: ./nom_ejecutable <TamañoMatriz> <NumProcesos>\n");
		exit(0);
	}

	int N = atoi(argv[1]);       // Dimensión de la matriz
	int num_P = atoi(argv[2]);   // Número de procesos
	double *matA = (double *) calloc(N * N, sizeof(double));
	double *matB = (double *) calloc(N * N, sizeof(double));
	double *matC = (double *) calloc(N * N, sizeof(double));

	srand(time(0)); 

	iniMatrix(matA, matB, N);
	impMatrix(matA, N);
	impMatrix(matB, N);

	int rows_per_process = N / num_P; // filas asignadas a cada proceso

	InicioMuestra(); // Comienza la medición de tiempo total

	for (int i = 0; i < num_P; i++) {
		pid_t pid = fork();
		
		if (pid == 0) { // Proceso hijo
			int start_row = i * rows_per_process;
			int end_row = (i == num_P - 1) ? N : start_row + rows_per_process;

			multiMatrix(matA, matB, matC, N, start_row, end_row);

			if (N < 9) {
				printf("\nChild PID %d calculó filas %d a %d:\n", getpid(), start_row, end_row - 1);
				for (int r = start_row; r < end_row; r++) {
					for (int c = 0; c < N; c++) {
						printf(" %.2f ", matC[N * r + c]);
					}
					printf("\n");
				}
			}
			exit(0); // Finaliza el hijo
		} 
		else if (pid < 0) {
			perror("Error al crear el proceso con fork");
			exit(1);
		}
	}

	// Esperar a que todos los hijos terminen
	for (int i = 0; i < num_P; i++) {
		wait(NULL);
	}
	
	FinMuestra(); // Finaliza medición y muestra el tiempo

	// Liberar memoria
	free(matA);
	free(matB);
	free(matC);

	return 0;
}
