/*
 * Pontificia Universidad Javeriana — Taller de Evaluación de Rendimiento
 * Autor original: J. Corredor, PhD
 * Documentado por: Santiago Bautista Velásquez
 * Fecha: 2025-11-10
 *
 * Descripción general:
 * ---------------------------------------------------------------
 * Implementación del algoritmo clásico de multiplicación de matrices
 * utilizando paralelismo con la biblioteca OpenMP.
 *
 * Cada hilo calcula un subconjunto de filas de la matriz resultado `mC`,
 * aprovechando múltiples núcleos del CPU mediante directivas de compilador
 * (`#pragma omp parallel` y `#pragma omp for`).
 *
 * Estructura del programa:
 *  - `iniMatrix()`: Inicializa matrices A y B con valores aleatorios.
 *  - `multiMatrix()`: Multiplica matrices usando paralelismo OpenMP.
 *  - `impMatrix()`: Imprime matrices pequeñas (N < 9).
 *  - `InicioMuestra()` / `FinMuestra()`: Miden el tiempo total de ejecución.
 *  - `main()`: Configura el entorno, ejecuta la multiplicación y muestra resultados.
 *
 * ---------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

struct timeval inicio, fin;

/*-----------------------------------------------------------------------------
 * InicioMuestra — Inicia el cronómetro de medición del rendimiento.
 *
 * Descripción:
 *  Registra el instante inicial utilizando `gettimeofday()` para calcular
 *  el tiempo total del bloque de ejecución.
 *---------------------------------------------------------------------------*/
void InicioMuestra() {
	gettimeofday(&inicio, (void *)0);
}

/*-----------------------------------------------------------------------------
 * FinMuestra — Finaliza la medición y muestra el tiempo total.
 *
 * Descripción:
 *  Calcula la diferencia entre los valores almacenados en `inicio` y `fin`
 *  en microsegundos, e imprime el resultado en pantalla.
 *---------------------------------------------------------------------------*/
void FinMuestra() {
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double)(fin.tv_sec * 1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

/*-----------------------------------------------------------------------------
 * impMatrix — Imprime una matriz cuadrada si el tamaño es pequeño (N < 9).
 *
 * Parámetros:
 *  - matrix: puntero al arreglo lineal que representa la matriz.
 *  - D: dimensión (tamaño de la matriz D×D).
 *
 * Descripción:
 *  Muestra los elementos en formato flotante con dos decimales. Se utiliza
 *  principalmente para depuración o verificación de resultados.
 *---------------------------------------------------------------------------*/
void impMatrix(double *matrix, int D) {
	if (D < 9) {
		printf("\n");
		for (int i = 0; i < D * D; i++) {
			if (i % D == 0) printf("\n");
			printf("%.2f ", matrix[i]);
		}
		printf("\n**-----------------------------**\n");
	}
}

/*-----------------------------------------------------------------------------
 * iniMatrix — Inicializa matrices A y B con valores aleatorios.
 *
 * Parámetros:
 *  - m1: puntero a la matriz A.
 *  - m2: puntero a la matriz B.
 *  - D:  dimensión de las matrices cuadradas.
 *
 * Descripción:
 *  Genera valores flotantes pseudoaleatorios:
 *   - A: valores entre 1.0 y 5.0
 *   - B: valores entre 2.0 y 9.0
 *---------------------------------------------------------------------------*/
void iniMatrix(double *m1, double *m2, int D) {
	for (int i = 0; i < D * D; i++, m1++, m2++) {
		*m1 = (double)rand() / RAND_MAX * (5.0 - 1.0);
		*m2 = (double)rand() / RAND_MAX * (9.0 - 2.0);
	}
}

/*-----------------------------------------------------------------------------
 * multiMatrix — Multiplica matrices usando paralelismo OpenMP.
 *
 * Parámetros:
 *  - mA: puntero a la matriz A.
 *  - mB: puntero a la matriz B.
 *  - mC: puntero a la matriz resultado C.
 *  - D:  dimensión de las matrices cuadradas.
 *
 * Descripción:
 *  Utiliza directivas OpenMP para distribuir el trabajo entre hilos. Cada hilo
 *  calcula las filas correspondientes de `mC`. La multiplicación se realiza
 *  utilizando el algoritmo clásico O(n³) en formato lineal.
 *
 * Notas:
 *  - La directiva `#pragma omp for` distribuye el bucle externo de `i`.
 *  - Cada hilo tiene su propia variable local `Suma`, evitando condiciones de carrera.
 *---------------------------------------------------------------------------*/
void multiMatrix(double *mA, double *mB, double *mC, int D) {
	double Suma, *pA, *pB;

	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < D; i++) {
			for (int j = 0; j < D; j++) {
				pA = mA + i * D;
				pB = mB + j;
				Suma = 0.0;

				for (int k = 0; k < D; k++, pA++, pB += D) {
					Suma += *pA * *pB;
				}
				mC[i * D + j] = Suma;
			}
		}
	}
}

/*-----------------------------------------------------------------------------
 * main — Función principal del programa.
 *
 * Parámetros:
 *  - argc: número de argumentos de la línea de comandos.
 *  - argv: arreglo de cadenas con los argumentos.
 *
 * Descripción:
 *  1. Valida la cantidad de parámetros.
 *  2. Reserva memoria para matrices A, B y C.
 *  3. Inicializa matrices con valores aleatorios.
 *  4. Configura el número de hilos con `omp_set_num_threads()`.
 *  5. Realiza la multiplicación y mide el tiempo total.
 *  6. Libera la memoria al finalizar.
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("\nUso: ./clasicaOpenMP <TamañoMatriz> <NumHilos>\n\n");
		exit(0);
	}

	int N = atoi(argv[1]);
	int TH = atoi(argv[2]);
	double *matrixA = (double *)calloc(N * N, sizeof(double));
	double *matrixB = (double *)calloc(N * N, sizeof(double));
	double *matrixC = (double *)calloc(N * N, sizeof(double));

	srand(time(NULL));
	omp_set_num_threads(TH);

	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	InicioMuestra();
	multiMatrix(matrixA, matrixB, matrixC, N);
	FinMuestra();

	impMatrix(matrixC, N);

	/* Liberación de memoria */
	free(matrixA);
	free(matrixB);
	free(matrixC);

	return 0;
}
