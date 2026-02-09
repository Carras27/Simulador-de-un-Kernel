//machine.c
#include "machine.h"

struct Machine maq;

uint64_t MemManagementUnit(uint64_t dirVirtual, struct TLB* tlb,  struct pagTab* PTBR)
{
  uint64_t pagina = dirVirtual / PAGE_SIZE;
  uint64_t offset = dirVirtual % PAGE_SIZE;

  //Buscar en la TLB
  for (int i = 0; i < tlb->num_entradas; i++)
  {
    if (tlb->entradas[i].valid && tlb->entradas[i].dir_virtual == pagina * PAGE_SIZE)
    {
      return  tlb->entradas[i].dir_fisica + offset;
    }
  }

  //No está en la TLB, consultar la tabla de páginas
  struct entradaTab entrada = PTBR->entradas[pagina];
  if (tlb->num_entradas < 64)
  {
    tlb->entradas[tlb->num_entradas].valid = 1;
    tlb->entradas[tlb->num_entradas].dir_virtual = pagina * PAGE_SIZE;
    tlb->entradas[tlb->num_entradas].dir_fisica = entrada.dir;
    tlb->num_entradas++;
  }
  return entrada.dir + offset;
}
