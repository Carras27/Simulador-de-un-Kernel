//synchronization.h
#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <pthread.h>
#include "common.h"

//Variables para controlar los cerrojos
extern pthread_mutex_t mutex_timer;
extern pthread_mutex_t mutex_cola;
extern pthread_mutex_t mutex_estado;
extern pthread_mutex_t mutex_clock;
extern pthread_mutex_t mutex_core[MAX_CORES];
extern pthread_mutex_t mutex_memoria;

extern pthread_t  id_clock, id_timer, id_load, id_sd, id_core[MAX_CORES]; //Identificadores de los "pthread"
extern pthread_cond_t cond_reloj, cond_timer, cond_scheduler, cond_loader, cond_core[MAX_CORES]; //Variables de condición para la sincronización

#endif
