#include "paralelismo.h"

void * crearBloqueMemoriaCompartida(void * memoria, size_t tam){
    int shm_fd;
    shm_fd = shm_open(NOMBRE_MEMORIA, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        return NULL;
    }
    if(ftruncate(shm_fd, tam) == -1){
        close(shm_fd);
        return NULL;
    }
    void * ptr = mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED){
        close(shm_fd);
        return NULL;
    }
    close(shm_fd);

    memcpy(ptr, memoria, tam);

    return ptr;
}

void liberarBloqueMemoriaCompartida(void * ptr, size_t tam){
    munmap(ptr, tam);
    shm_unlink(NOMBRE_MEMORIA);
}

void tareaHijo1(void * ptr, char * categoria){
    memoriaCompartida * memoria = (memoriaCompartida *)ptr;
    int i = 0;
    while(!memoria->fin){
        if(memoria->fin) break;
        usleep(DEMORA_H1 * SEGUNDO_MS);
        if(memoria->fin) break;
        sem_wait(memoria->semaforo);
        if(memoria->fin) {
            sem_post(memoria->semaforo);
            break;
        }
        // inicio accion
        Producto * producto = &memoria->vector.productos[i];
        if(strcmp(producto->categoria, categoria) == 0){
            int stockAnterior = producto->stock;
            if(stockAnterior > 0) {
                int cantidadVenta = (rand() % stockAnterior) + 1; // Entre 1 y stock actual
                producto->stock -= cantidadVenta;
                printf("Hijo 1: Venta simulada - Producto: %s\n", producto->nombre);
                fflush(stdout);
                printf("  Stock anterior: %d\n", stockAnterior);
                fflush(stdout);
                printf("  Stock nuevo: %d\n", producto->stock);
                fflush(stdout);
            }
        }
        // fin accion   
        sem_post(memoria->semaforo);
        if(i == memoria->vector.cant - 1){
            i = 0;
            printf("REINICIO == Hijo 1\n");
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
            nuevoStock = MIN_STOCK_AVISO + (rand() % (MAX_STOCK_REPOSICION * 2 - MIN_STOCK_AVISO + 1));
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