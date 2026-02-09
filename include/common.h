//common.h
//fichero con todas las definiciones de variables globales
#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define CLOCK_FREQ 1000 //1kHz, 1 tick cada 1 ms
#define MEMORY_SIZE (64 * 1024 * 1024) //64 MiB
#define PART_SIZE 512 //512B - el número total de particiones sera (64MiB / 512B = 131072)
#define KERNEL_SIZE (64* 1024) //64KiB reservados para el kernel en memoria fisica,
#define KERNEL_PART (KERNEL_SIZE / PART_SIZE); //64KiB/512B = 128
#define QUANTUM 4 //ticks de reloj
#define MAX_CORES 64 //maximo de cores (nCPU * nCore)
//Posibles estados de los procesos
#define FINALIZADO 2
#define PROCESANDO 1
#define READY 0


#endif
