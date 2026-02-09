//timers.c

#include "timers.h"

int tiempo_actual = 0;

//RELOJ
void *reloj(void* args)
{
  while (1)
  {
    int sleep_ms = 1000000 / CLOCK_FREQ; //Calcular frecuencia en microsegundos
    usleep(sleep_ms); //Esperar tantos microsegundos
    pthread_mutex_lock(&mutex_clock);
    pthread_cond_broadcast(&cond_reloj); //Enviar tick al timer, y a todos los cores de la máquina
    pthread_mutex_unlock(&mutex_clock);
  }
  return NULL;
}

//TEMPORIZADOR
void *temporizador(void* args)
{
  int freq = *(int*)args;
  int ciclos_tick = CLOCK_FREQ / freq; //Calculamos cada cuantos ticks del reloj tiene que enviar uno el timer
  int ciclos = 0;
  while (1)
  {
    //Espera a la señal del reloj
    pthread_mutex_lock(&mutex_clock);
    pthread_cond_wait(&cond_reloj, &mutex_clock);
    pthread_mutex_unlock(&mutex_clock);
    ciclos++;
    //Envía  una señal al resto de hilos del sistema, cada X ticks
    if (ciclos % ciclos_tick == 0)
    {
      pthread_mutex_lock(&mutex_timer);
      pthread_cond_signal(&cond_loader);
      pthread_cond_signal(&cond_scheduler);
      pthread_mutex_unlock(&mutex_timer);
    }
  }
  return NULL;
}
