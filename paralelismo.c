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

