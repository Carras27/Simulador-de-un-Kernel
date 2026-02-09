//loader.c

#include "loader.h"

#define KERNEL_PART 128

//LOADER
void *loader(void *args)
{
  DIR *dir = opendir("Secondary_Storage"); //Abre el Secondary Storage
  struct dirent *entry;
  int nPid = 1; //Nuemro a asignar al proximo proceso
  int i;

  //Recorrer el Secondary Storage
  while((entry = readdir(dir)) !=NULL)
  {
    pthread_mutex_lock(&mutex_timer);
    pthread_cond_wait(&cond_loader, &mutex_timer); //Espera al tick del timer
    pthread_mutex_unlock(&mutex_timer);

    //Procesar solo los programas ".elf"
    if (strstr(entry->d_name, ".elf"))
    {
      char filepath[512];
      snprintf(filepath, sizeof(filepath), "Secondary_Storage/%s", entry->d_name); //Obtiene la dirección del programa en el arbol de directorios
      FILE *file = fopen(filepath, "r"); //Abre el fichero del programa en modo lectura
      if (file == NULL)
      {
	fprintf(stderr, "Error abriendo archivo: %s\n", filepath);
	continue;
      }

      char line[256];
      size_t text_start = 0, data_start = 0; //Guardarán las direcciones de inicio de los segmentos de código y datos
      int  nLineas = 0; //El número de lineas del programa
      unsigned char codigo[256][4]; //Variable para guardar la informacion del programa, en palabras de 4 bytes
      //Recorrer el fichero del programa
      while (fgets(line, sizeof(line), file))
      {
	if (strstr(line, ".text")) {sscanf(line, ".text %lx", &text_start);} //Obtiene la direccion de inicio del segmento de código
	else if (strstr(line, ".data")) {sscanf(line, ".data %lx", &data_start);} //Obtiene la direccion de inicio del segmento de datos
	else { sscanf(line, "%x", (unsigned int *)&codigo[nLineas]) ;nLineas++; } //Obtiene las líneas y el número de lineas del programa
      }
      fclose(file); //Cierra el fichero del programa

      //Crear PCB
      struct PCB nuevo_pcb;
      nuevo_pcb.pid = nPid;
      nPid++;
      nuevo_pcb.estado = 0;
      nuevo_pcb.quantum_restante = QUANTUM;
      nuevo_pcb.PC = 0;

      //Crear tabla de páginas en memoria física del kernel
      //Buscar partición de espacio del kernel para la tabla de páginas
      pthread_mutex_lock(&mutex_memoria);
      struct Particion *part_tabla = NULL;
      size_t tam_tabla = sizeof(struct pagTab) + (nLineas * sizeof(struct entradaTab));
      for (i = 0; i < KERNEL_PART; i++) // Buscar en particiones del kernel
      {
        if (maq.memFisica->part[i].ocupada == 0)
        {
          part_tabla = &maq.memFisica->part[i];
          break;
        }
      }
      if (part_tabla == NULL)
      {
        fprintf(stderr, "Error: No hay espacio en kernel para tabla de páginas\n");
        continue;
      }
      struct pagTab *tabla_paginas = (struct pagTab *)(maq.memFisica->datos + part_tabla->inicio);
      size_t num_pags = (nLineas * 4 + PAGE_SIZE - 1) / PAGE_SIZE; //Calcular el número de páginas necesarias, redondeando hacia arriba
      tabla_paginas->entradas = (struct entradaTab *)((uint8_t *)tabla_paginas + sizeof(struct pagTab)); //Reservar memoria para las entradas de la tabla de páginas
      tabla_paginas->num_pags = num_pags;
      part_tabla->ocupada = 1;

      //Inicializar la tabla de páginas
      for (i = 0; i < num_pags; i++)
      {
	tabla_paginas->entradas[i].v = 0;
	tabla_paginas->entradas[i].p = 0;
	tabla_paginas->entradas[i].r = 0;
	tabla_paginas->entradas[i].d = 0;
	tabla_paginas->entradas[i].dir = -1; //Dirección física aún no asignada
      }
      pthread_mutex_unlock(&mutex_memoria);
      //Actualizar los punteros del PCB
      nuevo_pcb.mm.pgb = (struct pagTab *)(maq.memFisica->datos + part_tabla->inicio);
      nuevo_pcb.mm.code = (int *)text_start;
      nuevo_pcb.mm.data = (int *)data_start;

      //Buscar la primera particion libre, ignorando las particiones del kernel
      pthread_mutex_lock(&mutex_memoria);
      struct Particion *part_libre = NULL;
      for (i = KERNEL_PART; i < maq.memFisica->nPart; i++)
      {
	if (maq.memFisica->part[i].ocupada == 0)
	{
	  part_libre = &maq.memFisica->part[i];
	  break;
	}
      }

      //Si encontramos una partición libre, la ocupamos y copiamos el programa
      if (part_libre != NULL)
      {
	memcpy(maq.memFisica->datos + part_libre->inicio, codigo, 4 * nLineas);

	//Actualizamos la tabla de páginas
        int paginas_necesarias = (nLineas * 4 + PAGE_SIZE -1) / PAGE_SIZE;
	for (i = 0; i < paginas_necesarias; i++)
	{
	  tabla_paginas->entradas[i].v = 1; //Valida
	  tabla_paginas->entradas[i].p = 1; //Presente en memoria
	  tabla_paginas->entradas[i].r = 0; //No accedida aun
	  tabla_paginas->entradas[i].d = 0; //No modificada
	  tabla_paginas->entradas[i].dir = part_libre->inicio + (i * PAGE_SIZE);
	}
	part_libre->ocupada = 1; //Marcar la particion como ocupada
        pthread_mutex_unlock(&mutex_memoria);

        printf("Proceso PID %d cargado:\n", nuevo_pcb.pid);
        printf("  - Tabla páginas (física): 0x%lx\n", (size_t)nuevo_pcb.mm.pgb);
        printf("  - Programa en memoria física: 0x%lx\n", part_libre->inicio);
        printf("  - Páginas usadas: %d\n", paginas_necesarias);
      }
      else
      {
        fprintf(stderr, "Error: No hay particiones libres para el proceso PID %d\n", nuevo_pcb.pid);
        // Liberar la tabla de páginas si no se pudo cargar el programa
        part_tabla->ocupada = 0;
        continue;
      }

      //Crear un nuevo nodo y añadirlo al final de la cola
      struct PQueue *nuevo = (struct PQueue *)malloc(sizeof(struct PQueue));
      nuevo->info = nuevo_pcb; //Asignar el PCB al nuevo nodo
      nuevo->sig = NULL;  //El nuevo nodo apuntará a NULL
     pthread_mutex_lock(&mutex_cola);
      if (final == NULL) //Si la cola está vacía
      {
        inicio = final = nuevo;
      }
      else
      {
        final->sig = nuevo;
        final = nuevo;
	final->sig = NULL;
      }
      printf("Proceso añadido a la cola. PID: %d\n", final->info.pid);
      pthread_mutex_unlock(&mutex_cola);
    }
  }
  closedir(dir);
  return NULL;
}
