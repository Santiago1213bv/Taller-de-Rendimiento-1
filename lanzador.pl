#!/usr/bin/perl
##########################################################################################
# Pontificia Universidad Javeriana — Taller de Evaluación de Rendimiento
# Autor original: J. Corredor, PhD
# Documentado y ajustado por: Santiago Bautista Velásquez
# Fecha: 2025-11-10
#
# Descripción general:
# ---------------------------------------------------------------
# Script en Perl que automatiza la ejecución de los programas de multiplicación
# de matrices implementados en C (Fork, Pthreads, OpenMP, Filas OpenMP).
#
# Su función principal es:
#   - Ejecutar cada versión del algoritmo con distintos tamaños de matriz (N)
#     y números de hilos/procesos (P).
#   - Repetir cada prueba varias veces (por defecto 30) para obtener datos estables.
#   - Guardar los tiempos medidos en archivos de salida con extensión `.dat`.
#
# Archivos generados:
#   Fork.dat, Posix.dat, OpenMP.dat, FilasOpenMP.dat
#
# Requisitos:
#   - Ejecutables compilados previamente con el Makefile.
#   - Permisos de ejecución en este script:  chmod +x lanzador.pl
#
# Ejecución:
#   ./lanzador.pl
#
##########################################################################################

use strict;
use warnings;

# Configuración general -------------------------------------------------------

# Número de repeticiones por experimento
my $reps = 30;

# Tamaños de matriz a evaluar
my @sizes = (100, 200, 400, 600, 1200, 2400);

# Números de hilos o procesos
my @threads = (1, 2, 4);

# Ejecutables (deben existir en el mismo directorio)
my %executables = (
    "Fork"	   => "./mmClasicaFork",
    "Posix"        => "./mmClasicaPosix",
    "OpenMP"	   => "./mmClasicaOpenMP",
    "FilasOpenMP"  => "./mmFilasOpenMP"	
);

# Directorio de salida
my $out_dir = "resultados";
mkdir $out_dir unless -d $out_dir;


# Bucle principal de experimentos --------------------------------------------
print "\n=== INICIO DE EJECUCIONES AUTOMATIZADAS ===\n";

foreach my $exe (keys %executables) {
    my $program = $executables{$exe};
    my $outfile = "$out_dir/$exe.dat";

    open(my $fh, '>', $outfile) or die "No se pudo crear $outfile: $!";

    print $fh "# Resultados de ejecución para $exe\n";
    print $fh "# Formato: N P Tiempo(µs)\n\n";

    foreach my $n (@sizes) {
        foreach my $p (@threads) {
            print "Ejecutando $exe con N=$n y P=$p ...\n";
            for (my $i = 1; $i <= $reps; $i++) {
                my $cmd = "$program $n $p";
                my $output = `$cmd`;
                chomp($output);
                print $fh "$n $p $output\n";
            }
        }
        print "-------------------------------------------\n";
    }

    close($fh);
    print "Archivo generado: $outfile\n";
}

print "\n=== FIN DE TODAS LAS EJECUCIONES ===\n";
print "Los resultados están almacenados en la carpeta '$out_dir/'.\n";
