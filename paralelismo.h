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

#define DEMORA_H1 2.0
#define DEMORA_H2 1.5
#define DEMORA_H3 1
#define DEMORA_H4 0.5
#define SEGUNDO_MS 1000000



#define MAX_STOCK_AVISO 40
#define MIN_STOCK_AVISO 5
#define MAX_STOCK_REPOSICION 50


#define NOMBRE_MEMORIA "miMemoria"


typedef struct{
    vectorProductos vector;
    sem_t * semaforo;
    bool fin;
}memoriaCompartida;

void tareaHijo1(void * ptr, char * categoria);
void tareaHijo2(void * ptr);
void tareaHijo3(void * ptr);
void tareaHijo4(void * ptr, char * categoria);

void tareaHijo(void * ptr, int numHijo, double demora, accion accion);
void accionImprimirNombre(void * e, void * aux);


void * crearBloqueMemoriaCompartida(void * memoria, size_t tam);

void liberarBloqueMemoriaCompartida(void * ptr, size_t tam);

#endif // PARALELISMO_H