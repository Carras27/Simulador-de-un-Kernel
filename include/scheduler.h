//scheduler.h
#ifndef SCHEDULER_H
#define SHCEDULER_H

#include "process.h"
#include "machine.h"
#include "memory.h"
#include "synchronization.h"
#include "common.h"

//Estructura para definir la información que se le pasa a cada "core" para que procesen un proceso
struct informacion
{
  int cpu; //ID de la CPU
  int core; //ID del "core"
  struct PCB *proceso; //PCB del proceso
};

void *schedulerDispatcher(void *args);
void limpiar_procesos();
void *coreWork (void *args);


#endif
