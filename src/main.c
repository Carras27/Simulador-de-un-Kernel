//main.c

#include "common.h"
#include "timers.h"
#include "machine.h"
#include "synchronization.h"
#include "loader.h"
#include "scheduler.h"
#include "process.h"

int main(int argc, char *argv[])
{

  if (argc != 4)
  {
        printf("Uso: %s <frecuencia del timer> <numero de CPUs> <numero de cores> \n", argv[0]);
        exit(0);
  }
  printf("Inicializando.....\n");


  int freq = atoi(argv[1]); //frecuencia del timer (en Hz)
  int nCPU = atoi(argv[2]);  //número de CPUs
  int nCore = atoi(argv[3]); //número de núcleos por CPU

  //Montar la memoria fisica
  inicializar_memoria_fisica(&maq) ;
  //Inicializar CPUs
  maq.nCPU = nCPU;
  maq.cpu = malloc(sizeof(struct CPU) * nCPU);
  //Montar las CPUs, núcleos e hilos
  int core_index;
  for (int i=0; i < nCPU; i++)
  {
    maq.cpu[i].cpuID = i;
    maq.cpu[i].nCores = nCore;
    maq.cpu[i].cores = malloc(sizeof(struct Core) * nCore); //Reservar memoria para los núcleos
    for (int j=0; j<nCore; j++)
    {
      core_index = i * nCore + j;
      maq.cpu[i].cores[j].coreID = j;
      maq.cpu[i].cores[j].ocupado = 0; //Indicar que todos los cores están libres
      maq.cpu[i].cores[j].hilo = malloc(sizeof(struct Hilo)); //Reservar memoria para los hilos "hardware"
      maq.cpu[i].cores[j].hilo->hiloID = core_index;
      maq.cpu[i].cores[j].hilo->TLB = malloc(sizeof(struct TLB));
      maq.cpu[i].cores[j].hilo->TLB->num_entradas = 0;
      maq.cpu[i].cores[j].hilo->TLB->entradas = calloc(TAM_TLB, sizeof(struct entrada_TLB));
      maq.cpu[i].cores[j].hilo->PTBR = NULL;

      pthread_mutex_init(&mutex_core[core_index], NULL);
      pthread_cond_init(&cond_core[core_index], NULL);
      struct CoreArgs *args = malloc(sizeof(struct CoreArgs));
      args->cpuID = i;
      args->coreID = j;
      printf("Creando coreWork para CPU %d, Core %d, index %d\n", i, j, core_index);
      pthread_create(&id_core[core_index], NULL, coreWork, (void *)args );
    }
  }
  sleep(2);
  //Generar un hilo ("pthread") para cada hilo principal del sistema
  pthread_create(&id_clock, NULL, reloj, NULL);
  pthread_create(&id_timer, NULL, temporizador, &freq);
  pthread_create(&id_load, NULL, loader, NULL);
  sleep(2);
  pthread_create(&id_sd, NULL, schedulerDispatcher, NULL);

  pthread_join(id_clock, NULL); //Que el main no finalice hasta que finalice el clock

}
