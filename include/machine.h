//machine.h
#ifndef MACHINE_H
#define MACHINE_H

#include "memory.h"
#include "common.h"

#define NUM_ENTRADAS 8
#define TAM_TLB 16

struct entrada_TLB
{
  uint64_t dir_virtual;
  uint64_t dir_fisica;
  int valid;
};
struct TLB
{
  struct entrada_TLB *entradas;
  int num_entradas;
};

//Estructura para definir los hilos "hardware"
struct Hilo
{
  int hiloID;
  struct TLB *TLB; //Guarda las direcciones físicas recientemente accedidas
  struct pagTab *PTBR; //Puntero a la tabla de páginas
};

//Estructura para definir los núcleos
struct Core
{
  int coreID;
  struct Hilo *hilo; //Puntero al hilo "hardware"
  int ocupado; //0 libre, 1 ocupado
  struct PCB *pcb;
};

//Estructura para definir la CPU
struct CPU
{
  int cpuID;
  int nCores; //Número de "cores" en la CPU
  struct Core *cores; //"Array" con los "cores" de la CPU
};

//Estructura para definir la propia máquina
struct Machine
{
  int nCPU; //Numero de CPUs de la máquina
  struct CPU *cpu; //"Array" con las CPUs de la máquina
  struct Mem_Fisica *memFisica; //Puntero a la memoria física
};
extern struct Machine maq;

//Estructura auxiliar para pasarle los argumentos a coreWork
struct CoreArgs
{
  int cpuID;
  int coreID;
};


//MMU
uint64_t MemManagementUnit(uint64_t dirVirtual, struct TLB* tlb,  struct pagTab* PTBR);

#endif
