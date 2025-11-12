/*
 * Pontificia Universidad Javeriana — Taller de Evaluación de Rendimiento
 * Autor original: J. Corredor, PhD
 * Documentado por: Santiago Bautista Velásquez
 * Fecha: 2025-11-10
 *
 * Descripción general:
 * ---------------------------------------------------------------
 * Implementación del algoritmo de multiplicación de matrices clásico,
 * optimizado mediante el uso de la **matriz transpuesta** y paralelismo
 * con **OpenMP** (modelo de hilos compartidos).
 *
 * En esta versión, la segunda matriz (`mB`) se trata como transpuesta
 * para mejorar la **localidad de memoria** y la **eficiencia en cache**.
 * 
 * Cada hilo ejecuta un subconjunto de filas de `mA`, mientras que accede
 * secuencialmente a las filas de `mB` (equivalentes a columnas originales),
 * reduciendo así los saltos en memoria y acelerando el cálculo.
 *
 * Estructura del programa:
 *  - `iniMatrix()`: Inicializa matrices A y B con valores aleatorios.
 *  - `impMatrix()`: Imprime matrices en diferentes modos (normal o transpuesta).
 *  - `multiMatrixTrans()`: Realiza la multiplicación paralela optimizada.
 *  - `InicioMuestra()` / `FinMuestra()`: Miden el tiempo total de ejecución.
 *  - `main()`: Controla la ejecución, configurando OpenMP y midiendo el rendimiento.
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
 * InicioMuestra — Inicia el cronómetro de medición de rendimiento.
 *
 * Descripción:
 *  Registra la marca de tiempo inicial utilizando `gettimeofday()`.
 *---------------------------------------------------------------------------*/
void InicioMuestra() {
	gettimeofday(&inicio, (void *)0);
}

/*-----------------------------------------------------------------------------
 * FinMuestra — Finaliza la medición y muestra el tiempo total.
 *
 * Descripción:
 *  Calcula el tiempo transcurrido en microsegundos entre `InicioMuestra()` y
 *  el momento actual, luego imprime el resultado en pantalla.
 *---------------------------------------------------------------------------*/
void FinMuestra() {
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double)(fin.tv_sec * 1000000 + fin.tv_usec); 
	printf("%9.0f \n", tiempo);
}

/*-----------------------------------------------------------------------------
 * impMatrix — Imprime una matriz (normal o transpuesta) si el tamaño es pequeño.
 *
 * Parámetros:
 *  - matrix: puntero a la matriz lineal.
 *  - D: dimensión (tamaño) de la matriz cuadrada.
 *  - t: tipo de impresión
 *        0 → impresión normal (filas)
 *        1 → impresión transpuesta (columnas)
 *
 * Descripción:
 *  Muestra la matriz en formato flotante (2 decimales), útil para depuración
 *  y verificación visual de resultados en casos pequeños (D < 6).
 *---------------------------------------------------------------------------*/
void impMatrix(double *matrix, int D, int t) {
	int aux = 0;
	if (D < 6)
		switch (t) {
			case 0:
				for (int i = 0; i < D * D; i++) {
					if (i % D == 0) printf("\n");
					printf("%.2f ", matrix[i]);
				}
				printf("\n  - \n");
				break;

			case 1:
				while (aux < D) {
					for (int i = aux; i < D * D; i += D)
						printf("%.2f ", matrix[i]);
					aux++;
					printf("\n");
				}	
				printf("\n  - \n");
				break;

			default:
				printf("Sin tipo de impresión definido\n");
		}
}

/*-----------------------------------------------------------------------------
 * iniMatrix — Inicializa matrices A y B con valores aleatorios.
 *
 * Parámetros:
 *  - m1: puntero a la matriz A.
 *  - m2: puntero a la matriz B.
 *  - D:  tamaño (dimensión) de las matrices cuadradas.
 *
 * Descripción:
 *  Asigna valores pseudoaleatorios en punto flotante:
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
 * multiMatrixTrans — Multiplicación optimizada usando la matriz transpuesta.
 *
 * Parámetros:
 *  - mA: puntero a la matriz A.
 *  - mB: puntero a la matriz B (interpretada como transpuesta).
 *  - mC: puntero a la matriz resultado C.
 *  - D:  dimensión de las matrices.
 *
 * Descripción:
 *  Cada hilo OpenMP calcula un subconjunto de filas de `mC`. 
 *  La diferencia frente a la versión clásica es que `mB` se recorre por filas
 *  (en lugar de columnas), mejorando la localidad espacial en la memoria.
 *
 * Directivas OpenMP:
 *  - `#pragma omp parallel` crea el grupo de hilos.
 *  - `#pragma omp for` divide el bucle principal de filas `i` entre los hilos.
 *---------------------------------------------------------------------------*/
void multiMatrixTrans(double *mA, double *mB, double *mC, int D) {
	double Suma, *pA, *pB;

	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < D; i++) {
			for (int j = 0; j < D; j++) {
				pA = mA + i * D;	
				pB = mB + j * D;	
				Suma = 0.0;

				for (int k = 0; k < D; k++, pA++, pB++) {
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
 *  - argc: número de argumentos.
 *  - argv: arreglo de argumentos (argv[1]=tamaño, argv[2]=hilos).
 *
 * Descripción:
 *  1. Valida los argumentos de entrada.
 *  2. Reserva memoria dinámica para matrices A, B y C.
 *  3. Inicializa matrices con valores aleatorios.
 *  4. Configura el número de hilos con `omp_set_num_threads()`.
 *  5. Ejecuta la multiplicación optimizada y mide el tiempo total.
 *  6. Imprime resultados si la matriz es pequeña.
 *  7. Libera la memoria asignada.
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("\nUso: ./mmFilasOpenMP <TamañoMatriz> <NumHilos>\n\n");
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

	impMatrix(matrixA, N, 0);  // matriz normal
	impMatrix(matrixB, N, 1);  // matriz transpuesta

	InicioMuestra();
	multiMatrixTrans(matrixA, matrixB, matrixC, N);
	FinMuestra();

	impMatrix(matrixC, N, 0);

	/* Liberación de memoria */
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
