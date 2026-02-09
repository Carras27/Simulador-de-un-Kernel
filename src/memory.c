//memory.c

#include "memory.h"


//Incialización de memoria
void inicializar_memoria_fisica(struct Machine *maq)
{
  maq->memFisica = malloc(sizeof(struct Mem_Fisica));
  maq->memFisica->tam = MEMORY_SIZE;
  maq->memFisica->nPart = MEMORY_SIZE / PART_SIZE;
  maq->memFisica->part = malloc(maq->memFisica->nPart * sizeof(struct Particion));
  maq->memFisica->datos = calloc(MEMORY_SIZE, sizeof(uint8_t)); //Memoria fisica inicializada a 0

  for (int i = 0; i < maq->memFisica->nPart; i++)
  {
    maq->memFisica->part[i].inicio = i * PART_SIZE;
    maq->memFisica->part[i].tam = PART_SIZE;
    maq->memFisica->part[i].reg_base = i;
    maq->memFisica->part[i].ocupada = 0;
//Las primeras particiones están reservadas para el kernel
  }
}

