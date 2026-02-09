//timers.h
#ifndef TIMERS_H
#define TIMERS_H

#include "common.h"
#include "machine.h"
#include "synchronization.h"
#include "process.h"
#include "memory.h"

void *reloj(void* args);
void *temporizador(void* args);

#endif
