/*
 * Pontificia Universidad Javeriana — Taller de Evaluación de Rendimiento
 * Autor original: J. Corredor, PhD
 * Documentado por: Santiago Bautista Velásquez
 * Fecha: 2025-11-10
 *
 * Descripción general:
 * ---------------------------------------------------------------
 * Implementación del algoritmo clásico de multiplicación de matrices,
 * utilizando paralelismo con **hilos POSIX (Pthreads)**.
 *
 * Cada hilo ejecuta una porción del cálculo de la matriz resultado `matrixC`,
 * procesando un conjunto de filas determinado en función de su identificador.
 *
 * Estructura del programa:
 *  - `iniMatrix()`: Inicializa matrices A y B con valores aleatorios.
 *  - `impMatrix()`: Imprime matrices pequeñas (N < 9).
 *  - `multiMatrix()`: Función que ejecuta cada hilo para calcular su bloque.
 *  - `InicioMuestra()` y `FinMuestra()`: Miden el tiempo total en microsegundos.
 *  - `main()`: Configura hilos, crea y sincroniza la ejecución, libera recursos.
 *
 * ---------------------------------------------------------------
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/*-----------------------------------------------------------------------------
 * Variables globales:
 *  - Mutex: controla acceso concurrente (aunque aquí no se usa intensivamente)
 *  - Matrices: A, B, C son globales para que todos los hilos puedan acceder.
 *---------------------------------------------------------------------------*/
pthread_mutex_t MM_mutex;
double *matrixA, *matrixB, *matrixC;

/*-----------------------------------------------------------------------------
 * Estructura de parámetros:
 *  Contiene los datos que cada hilo necesita para su ejecución.
 *  - nH: número total de hilos.
 *  - idH: identificador del hilo (0, 1, 2...).
 *  - N: dimensión de las matrices cuadradas.
 *---------------------------------------------------------------------------*/
struct parametros {
	int nH;
	int idH;
	int N;
};

struct timeval inicio, fin;

/*-----------------------------------------------------------------------------
 * InicioMuestra — Marca el inicio del tiempo de medición.
 *---------------------------------------------------------------------------*/
void InicioMuestra() {
	gettimeofday(&inicio, (void *)0);
}

/*-----------------------------------------------------------------------------
 * FinMuestra — Marca el fin del tiempo de medición y muestra el resultado.
 *---------------------------------------------------------------------------*/
void FinMuestra() {
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double)(fin.tv_sec * 1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

/*-----------------------------------------------------------------------------
 * iniMatrix — Inicializa matrices A y B con valores aleatorios.
 *
 * Parámetros:
 *  - m1: puntero a la matriz A
 *  - m2: puntero a la matriz B
 *  - D:  tamaño de las matrices cuadradas
 *
 * Descripción:
 *  Asigna valores flotantes pseudoaleatorios:
 *   - A: valores entre 1.0 y 5.0
 *   - B: valores entre 5.0 y 9.0
 *---------------------------------------------------------------------------*/
void iniMatrix(double *m1, double *m2, int D) { 
	for (int i = 0; i < D * D; i++, m1++, m2++) {
		*m1 = (double)rand() / RAND_MAX * (5.0 - 1.0); 
		*m2 = (double)rand() / RAND_MAX * (9.0 - 5.0); 
	}	
}

/*-----------------------------------------------------------------------------
 * impMatrix — Imprime una matriz cuadrada si el tamaño es pequeño (N < 9).
 *
 * Parámetros:
 *  - matriz: puntero a la matriz lineal
 *  - D: dimensión de la matriz
 *---------------------------------------------------------------------------*/
void impMatrix(double *matriz, int D) {
	if (D < 9) {
    	for (int i = 0; i < D * D; i++) {
     		if (i % D == 0) printf("\n");
            printf(" %.2f ", matriz[i]);
		}	
    	printf("\n>-------------------->\n");
	}
}

/*-----------------------------------------------------------------------------
 * multiMatrix — Función ejecutada por cada hilo (cálculo parcial de la matriz).
 *
 * Parámetros:
 *  - variables: puntero genérico (void *) con los parámetros del hilo.
 *
 * Descripción:
 *  Cada hilo calcula las filas asignadas según su id (`idH`), con base en el
 *  número total de hilos (`nH`). Se utiliza un esquema de reparto de trabajo
 *  por bloques de filas.
 *
 *  El cálculo sigue el algoritmo clásico O(n³), pero dividido entre hilos.
 *---------------------------------------------------------------------------*/
void *multiMatrix(void *variables) {
	struct parametros *data = (struct parametros *)variables;
	
	int idH	 = data->idH;
	int nH	 = data->nH;
	int D	 = data->N;
	int filaI = (D / nH) * idH;
	int filaF = (D / nH) * (idH + 1);
	double Suma, *pA, *pB;

    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < D; j++) {
			pA = matrixA + i * D; 
			pB = matrixB + j;
			Suma = 0.0;

            for (int k = 0; k < D; k++, pA++, pB += D) {
				Suma += *pA * *pB;
			}
			matrixC[i * D + j] = Suma;
		}
	}

	/* Mutex no esencial aquí, pero se incluye como práctica segura */
	pthread_mutex_lock(&MM_mutex);
	pthread_mutex_unlock(&MM_mutex);

	pthread_exit(NULL);
}

/*-----------------------------------------------------------------------------
 * main — Función principal del programa.
 *
 * Parámetros:
 *  - argc: número de argumentos pasados por línea de comandos
 *  - argv: arreglo de argumentos
 *
 * Descripción:
 *  1. Valida argumentos de entrada.
 *  2. Reserva memoria dinámica para matrices.
 *  3. Inicializa e imprime matrices (si N < 9).
 *  4. Crea hilos POSIX y les asigna rangos de filas.
 *  5. Espera la finalización de todos los hilos (`pthread_join`).
 *  6. Calcula y muestra el tiempo total.
 *  7. Libera la memoria y destruye recursos de hilos.
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Uso: ./mmClasicaPosix <tamMatriz> <numHilos>\n");
		exit(0);	
	}

    int N = atoi(argv[1]); 
    int n_threads = atoi(argv[2]); 

    pthread_t p[n_threads];
    pthread_attr_t atrMM;

	matrixA = (double *)calloc(N * N, sizeof(double));
	matrixB = (double *)calloc(N * N, sizeof(double));
	matrixC = (double *)calloc(N * N, sizeof(double));

	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	InicioMuestra();

	pthread_mutex_init(&MM_mutex, NULL);
	pthread_attr_init(&atrMM);
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

    for (int j = 0; j < n_threads; j++) {
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros)); 
		datos->idH = j;
		datos->nH  = n_threads;
		datos->N   = N;

        pthread_create(&p[j], &atrMM, multiMatrix, (void *)datos);
	}

    for (int j = 0; j < n_threads; j++)
        pthread_join(p[j], NULL);

	FinMuestra();
	
	impMatrix(matrixC, N);

	/* Liberación de Memoria */
	free(matrixA);
	free(matrixB);
	free(matrixC);

	pthread_attr_destroy(&atrMM);
	pthread_mutex_destroy(&MM_mutex);
	pthread_exit(NULL);

	return 0;
}
