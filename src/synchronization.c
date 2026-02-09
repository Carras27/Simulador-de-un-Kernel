//synchronization.c

#include "synchronization.h"

pthread_mutex_t mutex_timer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_estado = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_core[MAX_CORES];
pthread_mutex_t mutex_clock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_memoria = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_reloj = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_timer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_scheduler = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_loader = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_core[MAX_CORES];

pthread_t  id_clock, id_timer, id_load, id_sd, id_core[MAX_CORES];
