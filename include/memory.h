//memory.h

#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "machine.h"

#define PAGE_SIZE 512 //512B
#define PAGE_ENTRIES 4
#define PAGE_TABLE_SIZE (PAGE_ENTRIES * 4) //32B por tabla

//Estructura para definir las entradas de la tabla de páginas
struct entradaTab
{
  int v; //Indica si la página la usa el proceso
  int p; //Indica si la entrada está en memoria física o no
  int r; //Indica si la página ha sido accedida
  int d; //Indica si la página ha sido modificada
  uint64_t dir; //Dirección física
};

//Estructura para definir la tabla de páginas
struct pagTab
{
  struct entradaTab *entradas; //Punteros a todas las entradas en la tabla
  size_t num_pags; //Número total de páginas virtuales
};

//Estructara para definir las particiones
struct Particion
{
  size_t inicio; //Dirección de inicio de la partición
  size_t tam; //Tamaño de la partición
  int reg_base; //Registro base
  int ocupada; //0 libre, 1 ocupada
};

//Estructura para definir la memoria física
struct Mem_Fisica
{
  size_t tam; //Tamaño de la memoria física
  int nPart; //Número de particiones
  struct Particion *part; //Particiones de la memoria física
  uint8_t *datos; //Memoria fisica real
};


//Gestión de memoria
void inicializar_memoria_fisica();
void liberar_memoria_fisica();

#endif
