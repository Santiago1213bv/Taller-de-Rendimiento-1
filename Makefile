###############################################################################
# Pontificia Universidad Javeriana
# Taller de Evaluación de Rendimiento — Makefile General
# Autor original: J. Corredor, PhD
# Ajustado y documentado por: Santiago Bautista Velásquez
# Fecha: 2025-11-10
#
# Descripción:
# Compila las cuatro versiones del algoritmo de multiplicación de matrices:
#   1. mmClasicaFork.c       → Paralelismo con procesos fork()
#   2. mmClasicaPosix.c      → Paralelismo con hilos POSIX (pthreads)
#   3. mmClasicaOpenMP.c     → Paralelismo con OpenMP
#   4. mmFilasOpenMP.c       → Multiplicación optimizada (filas × filas)
#
# Comandos:
#   make all       → Compila todas las versiones
#   make clean     → Elimina ejecutables
#
# Ejemplos de ejecución:
#   ./mmClasicaFork 600 4
#   ./mmClasicaPosix 600 4
#   ./mmClasicaOpenMP 600 4
#   ./mmFilasOpenMP 600 4
###############################################################################

# Compilador
CC = gcc

# Opciones de compilación
CFLAGS = -O2 -Wall

# Archivos fuente
SRC_FORK    = mmClasicaFork.c
SRC_POSIX   = mmClasicaPosix.c
SRC_OPENMP  = mmClasicaOpenMP.c
SRC_FILAS   = mmFilasOpenMP.c

# Ejecutables resultantes
BIN_FORK    = mmClasicaFork
BIN_POSIX   = mmClasicaPosix
BIN_OPENMP  = mmClasicaOpenMP
BIN_FILAS   = mmFilasOpenMP

# Regla principal: compila todo
all: $(BIN_FORK) $(BIN_POSIX) $(BIN_OPENMP) $(BIN_FILAS)
	@echo "✅ Compilación completa. Ejecutables listos."

# Versión Fork (procesos)
$(BIN_FORK): $(SRC_FORK)
	$(CC) $(CFLAGS) -o $@ $^

# Versión POSIX (hilos)
$(BIN_POSIX): $(SRC_POSIX)
	$(CC) $(CFLAGS) -pthread -o $@ $^

# Versión OpenMP (clásica)
$(BIN_OPENMP): $(SRC_OPENMP)
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

# Versión OpenMP por filas (transpuesta)
$(BIN_FILAS): $(SRC_FILAS)
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

# Limpieza de ejecutables
clean:
	rm -f $(BIN_FORK) $(BIN_POSIX) $(BIN_OPENMP) $(BIN_FILAS)
	@echo "🧹 Archivos compilados eliminados."
