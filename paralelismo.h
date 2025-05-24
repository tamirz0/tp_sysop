#ifndef PARALELISMO_H
#define PARALELISMO_H

#include "producto.h"
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define ERR_SHM -2


#define NOMBRE_MEMORIA "miMemoria"

void * crearBloqueMemoriaCompartida(void * memoria, size_t tam);

void liberarBloqueMemoriaCompartida(void * ptr, size_t tam);

#endif // PARALELISMO_H