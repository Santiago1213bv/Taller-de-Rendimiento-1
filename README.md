Taller de Evaluación de Rendimiento – POSIX y OpenMP

Sistemas Operativos – Noviembre 2025
Autor: Santiago Bautista Velásquez

Descripción General

Este repositorio contiene el desarrollo completo del Taller de Evaluación de Rendimiento, cuyo propósito es analizar y comparar diferentes técnicas de paralelización aplicadas al algoritmo de multiplicación clásica de matrices.
Las estrategias evaluadas incluyen procesos POSIX (Fork), hilos POSIX (Pthreads), OpenMP y una versión optimizada con paralelización por filas mediante OpenMP.

El repositorio reúne el código fuente, el lanzador automatizado de pruebas, los archivos de resultados generados, la hoja de cálculo con análisis estadísticos y gráficas, así como el informe final del taller.

Estructura del Repositorio

mmClasicaFork.c
mmClasicaPosix.c
mmClasicaOpenMP.c
mmFilasOpenMP.c
lanzador.sh
Makefile
Fork.dat
Posix.dat
OpenMP.dat
FilasOpenMP.dat
Pruebas-Taller-Rendimiento.xlsx
informe.pdf
README.md

Contenido de los archivos principales

mmClasicaFork.c
Implementación del algoritmo de multiplicación de matrices mediante procesos POSIX utilizando fork.

mmClasicaPosix.c
Implementación con hilos POSIX (pthread), compartiendo memoria entre los hilos.

mmClasicaOpenMP.c
Implementación paralela utilizando OpenMP y directivas pragmas para distribuir la carga computacional.

mmFilasOpenMP.c
Versión optimizada con OpenMP que reparte el cálculo por filas, mejorando la localidad de memoria.

lanzador.sh
Script automatizado que compila todos los programas y ejecuta las pruebas para múltiples tamaños de matriz y números de hilos. Genera los archivos .dat con los tiempos de ejecución.

Makefile
Archivo de construcción que permite compilar cada implementación o todas en conjunto.

Fork.dat, Posix.dat, OpenMP.dat, FilasOpenMP.dat
Archivos generados automáticamente con los resultados de tiempo en microsegundos para cada método de paralelización.

Pruebas-Taller-Rendimiento.xlsx
Documento con el análisis estadístico, tablas resumen y gráficas de eficiencia, speedup y tiempos de ejecución.

informe.pdf
Documento final del taller que contiene metodología, análisis, conclusiones y resultados comparativos.

Compilación y Ejecución

Compilar todos los programas:

make

Compilar un programa específico:

make mmClasicaFork
make mmClasicaPosix
make mmClasicaOpenMP
make mmFilasOpenMP

Ejecutar manualmente un programa:

./mmClasicaFork N P
./mmClasicaPosix N P
./mmClasicaOpenMP N P
./mmFilasOpenMP N P

N corresponde al tamaño de la matriz y P al número de hilos o procesos utilizados.

Ejecución Automática

El script lanzador.sh permite ejecutar todas las pruebas del taller de forma automatizada.

Dar permisos al script:

chmod +x lanzador.h

Ejecutar todas las pruebas:

./lanzador.h

Se generan automáticamente los archivos Fork.dat, Posix.dat, OpenMP.dat y FilasOpenMP.dat con el formato:

N P tiempo_en_microsegundos

Contenido del Taller

A. Informe
Documento PDF con la descripción del desarrollo del taller, evidencias, análisis de resultados y conclusiones.

B. Identificación y Descripción del Código
Cada archivo fuente incluye comentarios explicativos sobre el objetivo del programa, las variables utilizadas y el flujo de ejecución.

C. Organización e Implementación
El repositorio presenta una estructura organizada, un Makefile central y código modular por implementación.

D. Documentación del Código
Cada programa incluye documentación de funciones, comentarios por bloques lógicos y explicación del uso de procesos, hilos, sincronización o directivas de paralelización.

E. Análisis Experimental
Se presenta por cada técnica la descripción del experimento, la solución implementada, resultados estadísticos, gráficas de eficiencia y tiempos, además de los errores encontrados y su corrección.

F. Conclusiones
Se resumen los principales aprendizajes relacionados con la creación y gestión de procesos POSIX, uso de hilos, sincronización, análisis de eficiencia, speedup y comportamiento en distintos sistemas operativos.

G. Modularidad y Makefile
Se incluye un Makefile funcional con reglas para compilar toda la solución, programas individuales y limpiar archivos binarios.

H. Organización del Repositorio
El repositorio está estructurado para una fácil navegación, evaluación y replicación de las pruebas.

Autor

Santiago Bautista Velásquez
Pontificia Universidad Javeriana
Sistemas Operativos – 2025
