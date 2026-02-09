//process.h
#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"


//Estructura auxiliar para el PCB
struct MM
{
  int *code; //Puntero a la dirección virtual de comienzo del segmento de código
  int *data; //Puntero a la dirección virtual de comienzo del segmento de datos
  struct pagTab *pgb; //Puntero a la dirección física de la correspondiente tabla de páginas
};

//"Process Control Block"
struct PCB //
{
  int pid; //Identificador del proceso
  int estado; //Estado del proceso: 0 preparado, 1 procesando, 2 finalizado
  struct MM mm; //Variable de la estructura mm
  int quantum_restante;
  uint64_t PC; //La direccion de la proxima instruccion a ejecutar
  int r[16]; //Registros
};

//Estructura para definir la cola de procesos
struct PQueue //Cola de Procesos
{
  struct PCB info; //Información de cada elemento de la cola (Process Control Block)
  struct PQueue *sig; //Puntero al siguiente elemento de la cola
};

extern struct PQueue *inicio; //Puntero al inicio de la cola de procesos nuevos
extern struct PQueue *final; //Puntero al final de la cola de processo nuevos

#endif
