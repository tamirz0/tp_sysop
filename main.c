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

#define MAX_STOCK_AVISO 40
#define MIN_STOCK_AVISO 30
#define MAX_STOCK_REPOSICION 50

void tareaHijo(void * ptr, int numHijo, double demora, accion accion);
void tareaHijo2(void * ptr);
void tareaHijo3(void * ptr);
void tareaHijo4(void * ptr, char * categoria);

void accionImprimirNombre(void * e, void * aux);

int main(){
    // 1 Guardar los productos en un vector
    vectorProductos vector;
    leerCsvProductos(NOMBRE_ARCHIVO_CSV, &vector);

    // 2 Preparar la memoria compartida
    memoriaCompartida memoria;
    memoria.vector = vector;  // Ahora podemos copiar directamente
    memoria.fin = false;
    memoria.semaforo = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0600, 1);

    // 3 Crear el bloque de memoria compartida
    void * ptr = crearBloqueMemoriaCompartida(&memoria, sizeof(memoriaCompartida));
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
                tareaHijo2(ptr);
            }
            else if(i == 2){
                tareaHijo3(ptr);
            }
            else if(i == 3){
                tareaHijo4(ptr, "Accesorios");
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
    mostrarVectorProductos(&mem->vector);

    liberarBloqueMemoriaCompartida(ptr, sizeof(memoriaCompartida));
    sem_close(memoria.semaforo);
    sem_unlink(NOMBRE_SEMAFORO);

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

        // Inicio de la zona critica

        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        accion(&memoria->vector.productos[i], &numHijo);
        sem_post(memoria->semaforo);

        // Fin de la zona critica

        if(i == memoria->vector.cant - 1){
            i = 0;
            printf("REINICIO == Hijo %d\n", numHijo);
            fflush(stdout);
        }
        else{
            i++;
        }
    }
    exit(0);
}

void tareaHijo2(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(DEMORA_H2 * SEGUNDO_MS);
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        // inicio accion
        Producto * producto = &memoria->vector.productos[i];
        if(producto->stock >= MAX_STOCK_AVISO){
            printf("Hijo 2: Producto %s con SOBRESTOCK : %d\n", producto->nombre, producto->stock);
            fflush(stdout);
        }
        // fin accion   
        sem_post(memoria->semaforo);
        if(i == memoria->vector.cant - 1){
            i = 0;
            printf("REINICIO == Hijo 2\n");
            fflush(stdout);
        }
        else{
            i++;
        }
    }
    exit(0);
}

void tareaHijo3(void * ptr){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0, nuevoStock;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(DEMORA_H3 * SEGUNDO_MS);
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        // inicio accion
        Producto * producto = &memoria->vector.productos[i];
        if(producto->stock < MIN_STOCK_AVISO){
            nuevoStock = MIN_STOCK_AVISO + (rand() % (MAX_STOCK_REPOSICION - MIN_STOCK_AVISO + 1));
            printf("Hijo 3: Reposición de stock - Producto: %s\n", producto->nombre);
            fflush(stdout);
            printf("  Stock anterior: %d\n", producto->stock);
            fflush(stdout);
            producto->stock = nuevoStock;
            printf("  Stock nuevo: %d\n", producto->stock);
            fflush(stdout);
        }
        // fin accion   
        sem_post(memoria->semaforo);
        if(i == memoria->vector.cant - 1){
            i = 0;
            printf("REINICIO == Hijo 3\n");
            fflush(stdout);
        }
        else{
            i++;
        }
    }
    exit(0);
}

void tareaHijo4(void * ptr, char * categoria){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    double acumulador = 0;
    int cant = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(DEMORA_H4 * SEGUNDO_MS);
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        // inicio accion
        Producto * producto = &memoria->vector.productos[i];
        if(strcmp(producto->categoria, categoria) == 0){
            acumulador += (producto->precio * producto->stock);
            cant++;
        }
        // fin accion
        sem_post(memoria->semaforo);

        if(i == memoria->vector.cant - 1){
            i = 0;
            printf("REINICIO == Hijo %d\n", 4);
            fflush(stdout);
        }
        else{
            i++;
        }
    }
    printf("Promedio del valor en inventario por producto de la categoria %s: %.2lf\n", categoria, acumulador / cant);
    fflush(stdout);
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