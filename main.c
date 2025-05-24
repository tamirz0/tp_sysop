#include "producto.h"
#include "paralelismo.h"
#include <stdio.h>


/*
    Lee el archivo CSV y guarda todos los productos en un vector Producto[].
    Copia ese vector a un bloque de memoria compartida.
    Crea 4 procesos hijos con fork().
        Los 4 procesos hijos realizan sus respectivas tareas en paralelo
        En caso de utilizarse escritura en el vector se da lugar a un bloqueo de la memoria compartida a través de un semáforo para evitar condición de carrera.
    El usuario puede finalizar la ejecución en cualquier momento
        En caso de que se termine de recorrer el vector, automáticamente se comenzará a recorrer otra vez desde el inicio. 
    El padre espera a que los hijos terminen sus procesos.
    Libera recursos: memoria compartida y semáforos.
    Fin del sistema.
*/

typedef struct{
    vectorProductos vector;
    sem_t * semaforo;
    bool fin;
}memoriaCompartida;

#define NOMBRE_ARCHIVO_CSV "test.csv"
#define NOMBRE_SEMAFORO "miSemaforo"

#define DEMORA_H1 2.0
#define DEMORA_H2 1.5
#define DEMORA_H3 1
#define DEMORA_H4 0.5
#define SEGUNDO_MS 1000000


void tareaHijo(void * ptr, int numHijo, double demora, accion accion);

void accionImprimirNombre(void * e, void * aux);

int main(){
    // 1 Guardar los productos en un vector
    vectorProductos vector;
    leerCsvProductos(NOMBRE_ARCHIVO_CSV, &vector);
    
    // 2 Copiar el vector a un bloque de memoria compartida
    memoriaCompartida memoria;
    memoria.vector = vector;
    memoria.fin = false;
    memoria.semaforo = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0600, 1);

    // 3 Crear el bloque de memoria compartida

    void * ptr = crearBloqueMemoriaCompartida(&memoria , sizeof(memoriaCompartida));
    if(ptr == NULL){
        printf("Error al crear el bloque de memoria compartida\n");
        return 1;
    }

    // 4 Crear los 4 procesos hijos
    for(int i = 0; i < 4; i++){
        int hijo = fork();
        if(hijo == 0){
            if(i == 0){
                tareaHijo(ptr, 1, DEMORA_H1, accionImprimirNombre);
            }
            else if(i == 1){
                tareaHijo(ptr, 2, DEMORA_H2, accionImprimirNombre);
            }
            else if(i == 2){
                tareaHijo(ptr, 3, DEMORA_H3, accionImprimirNombre);
            }
            else if(i == 3){
                tareaHijo(ptr, 4, DEMORA_H4, accionImprimirNombre);
            }
            exit(0);
        }
    }
    
    printf("Presione Enter para finalizar...\n");
    getchar();
    
    // Indicar a los hijos que terminen
    memoriaCompartida * mem = (memoriaCompartida *)ptr;
    mem->fin = true;
    
    // Esperar a que todos los hijos terminen
    for(int i = 0; i < 4; i++) {
        wait(NULL);
    }
    printf("Todos los hijos terminaron\n");
    
    // Liberar la memoria compartida
    liberarBloqueMemoriaCompartida(ptr, sizeof(memoriaCompartida));
    sem_close(memoria.semaforo);
    sem_unlink(NOMBRE_SEMAFORO);
    liberarVectorProductos(&vector);

    return 0;
}

void accionImprimirNombre(void * e, void * aux){
    printf("Hijo %d: %s\n", *(int *)aux, ((Producto *)e)->nombre);
    fflush(stdout);
}

void tareaHijo(void * ptr, int numHijo, double demora, accion accion){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(demora * SEGUNDO_MS); // 2 segundos - proceso más lento
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        accion(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);
        i = (i + 1) % memoria->vector.cant;
    }
    exit(0);
}

/*
void tareaHijo1(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(2000000); // 2 segundos - proceso más lento
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        int numHijo = 1;
        accionImprimirNombre(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);
        i = (i + 1) % memoria->vector.cant;
    }
    exit(0);
}

void tareaHijo2(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(1500000); // 1.5 segundos
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        int numHijo = 2;
        accionImprimirNombre(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);
        i = (i + 1) % memoria->vector.cant;
    }
    exit(0);
}

void tareaHijo3(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(1000000); // 1 segundo
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        int numHijo = 3;
        accionImprimirNombre(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);
        i = (i + 1) % memoria->vector.cant;
    }
    exit(0);
}

void tareaHijo4(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(500000); // 0.5 segundos - proceso más rápido
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        int numHijo = 4;
        accionImprimirNombre(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);
        i = (i + 1) % memoria->vector.cant;
    }
    exit(0);
}
*/  