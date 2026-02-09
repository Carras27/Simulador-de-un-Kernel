//scheduler.c

#include "scheduler.h"

//SCHEDULER/DISPATCHER
void *schedulerDispatcher(void *args)
{
  int nCPU = maq.nCPU; //Número de CPUs
  int nCore = maq.cpu[0].nCores; //Número de "cores" por CPU
  int totalCores = nCPU * nCore; //Número total de "cores"
  pthread_t *id_hilos = malloc(totalCores * sizeof(pthread_t)); //Crea un "pthread_id" por cada "core"
  int indexCore; //Variable que almacenará el índice del "core"

  while (1)
  {
    pthread_mutex_lock(&mutex_timer);
    pthread_cond_wait(&cond_scheduler, &mutex_timer); //Espera la señalización del temporizador
    pthread_mutex_unlock(&mutex_timer);

    pthread_mutex_lock(&mutex_cola);
    //Si la cola de procesos está vacía, no hará nada
     if (inicio == NULL) {
        pthread_mutex_unlock(&mutex_cola);
        continue; // No hay procesos
    }
    struct PQueue *actual = inicio;
    inicio = inicio->sig;
    if (inicio == NULL) final = NULL;
    actual->sig = NULL;

    if (actual->info.estado == FINALIZADO) //Si el primer proceso de la cola está finalizado, lo borrará de la memoria.
    {
      inicio = actual->sig;  // El segundo nodo pasa a ser el primero
      if (inicio == NULL) {
         final = NULL;  // Si no hay más nodos, final también es NULL
      }
      pthread_mutex_unlock(&mutex_cola);
      free(actual);
      continue;
    }

    if (actual->info.estado == READY)
    {
       bool asignado = false;
       for (int i = 0; i < nCPU && !asignado; i++)
       {
       for (int j = 0; j < nCore && !asignado; j++)
       {
         pthread_mutex_lock(&mutex_core[i * nCore +j]);
         if (maq.cpu[i].cores[j].ocupado == 0)
         {
           maq.cpu[i].cores[j].hilo->PTBR = actual->info.mm.pgb; //El hilo apuntará a la tabla de páginas del proceso

           maq.cpu[i].cores[j].ocupado = 1; //Indicar que este "core" está ocupado
	   maq.cpu[i].cores[j].pcb = &actual->info;
	   actual->info.estado = PROCESANDO;  //y el proceso,  procesandose
	   actual->info.quantum_restante = QUANTUM;

           indexCore = i * nCore + j;

	   printf("Procesando core: CPU %d, Core %d, Proceso PID: %d\n", i, j, actual->info.pid);
	   asignado = true;
	   if (final != NULL) {
             final->sig = actual;
           } else {
              inicio = actual;
           }
           final = actual;
           final->sig = NULL;
         }
	 pthread_cond_signal(&cond_core[i * nCore + j]);
         pthread_mutex_unlock(&mutex_core[i * nCore + j]);
      }
      }
      pthread_mutex_unlock(&mutex_cola);
   }
  }
  return NULL;
}


void *coreWork (void *args)
{
  struct CoreArgs *data = (struct CoreArgs *)args;
  int cpuID = data->cpuID;
  int coreID = data->coreID;
  free(data);
  struct Core *core = &maq.cpu[cpuID].cores[coreID];
  struct PCB *proceso = NULL;
  int index = cpuID *maq.cpu[0].nCores + coreID;

  while (1)
  {
    pthread_mutex_lock(&mutex_core[index]);
    while (core->ocupado==0)
    {
      pthread_cond_wait(&cond_core[index], &mutex_core[index]);
    }
    pthread_mutex_unlock(&mutex_core[index]);
    proceso = core->pcb;
    while (proceso->quantum_restante > 0)
    {
      pthread_mutex_lock(&mutex_clock);
      pthread_cond_wait(&cond_reloj, &mutex_clock);
      pthread_mutex_unlock(&mutex_clock);

      uint64_t pc_virtual = proceso->PC; //obtener PC actual
      //Traducir direccion
      uint64_t dir_fisica = MemManagementUnit(pc_virtual, core->hilo->TLB, proceso->mm.pgb);
      //Leer instruccion de 4 bytes
      uint32_t instruccion = *((uint32_t *)(maq.memFisica->datos + dir_fisica));

      uint8_t opcode = (instruccion >> 28) & 0xF;
      uint8_t reg = (instruccion >> 24) & 0xF;
      uint32_t resto = instruccion & 0xFFFFFF;
      switch (opcode)
      {
        case 0x0: //Load
        {
          uint64_t dir_virt = resto;
          uint64_t dir_fis = MemManagementUnit(dir_virt, core->hilo->TLB, proceso->mm.pgb);
          pthread_mutex_lock(&mutex_memoria);
          proceso->r[reg] = *(int *)(maq.memFisica->datos + dir_fis);
          pthread_mutex_unlock(&mutex_memoria);
          printf("[Core %d:%d] Load: r[%d] <- mem[0x%lx]\n", cpuID, coreID, reg, dir_fis);
          break;
        }
        case 0x1: //Store
        {
          uint64_t dir_virt = resto;
          uint64_t dir_fis = MemManagementUnit(dir_virt, core->hilo->TLB, proceso->mm.pgb);
          pthread_mutex_lock(&mutex_memoria);
          *(int *)(maq.memFisica->datos + dir_fis) = proceso->r[reg];
          pthread_mutex_unlock(&mutex_memoria);
          printf("[Core %d:%d] Store: mem[0x%lx] <- r[%d]\n", cpuID, coreID, dir_fis, reg);
          break;
        }
        case 0x2: //Add
        {
          uint8_t r1 = (resto >> 8) & 0xF;
          uint8_t r2 = (resto >> 4) & 0xF;
          proceso->r[reg] = proceso->r[r1] + proceso->r[r2];
                    printf("[Core %d:%d] Add: r[%d] = r[%d] + r[%d]\n", cpuID, coreID, reg, r1, r2);
          break;
        }
        case 0xF: //Exit
        {
          proceso->estado = FINALIZADO;
          proceso->quantum_restante=0;
          core->ocupado=0;
          core->pcb = NULL;
          printf("[Core %d:%d] Proceso finalizado (PID %d)\n", cpuID, coreID, proceso->pid);
        }
      }
    proceso->PC += 4;
    proceso->quantum_restante--;

  }
  if (proceso->quantum_restante <= 0 && proceso->estado != FINALIZADO)
  {
    proceso->quantum_restante = QUANTUM;
    proceso->estado = READY;
    pthread_mutex_lock(&mutex_core[index]);
    core->ocupado=0;
    core->pcb = NULL;
    pthread_mutex_unlock(&mutex_core[index]);
  }
}
}
