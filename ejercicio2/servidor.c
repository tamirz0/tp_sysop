#include "lib_comunicacion/comunicacion.h"


#define PUERTO 5000

int socketServidor;
sem_t semaforoHilosDisponibles; // Semáforo para controlar la cantidad de hilos disponibles

int cantidadClientesAtendidos = 0; // Cantidad de clientes conectados (atendidos)
int cantidadClientesEnEspera = 0; // Cantidad de clientes en espera de ser atendidos
/*
    Clientes en espera necesita ser considerado para la finalizacion del servidor.
    Ya que si se finaliza un cliente atendido, y justo antes de que se atienda el siguiente cliente en espera,
    el monitorFinalizacion se ejecuta entonces se habria finalizado el servidor cuando no deberia.
*/
pthread_mutex_t mutexcantidadClientesAtendidos = PTHREAD_MUTEX_INITIALIZER; // Mutex para la cantidad de clientes

void * manejarCliente(void * arg);
void * monitorFinalizacion(void * arg);
void cerrarServidorSigInt(int sig);

int main(){
    signal(SIGINT, cerrarServidorSigInt);
    signal(SIGPIPE, SIG_IGN);
    socketServidor = crearSocketServidor();
    pthread_t thread[MAX_CLIENTES];
    pthread_t threadMonitor;
    bool primerCliente = true;
    sem_init(&semaforoHilosDisponibles, 0, MAX_CLIENTES);
    if (socketServidor == -1){
        perror("Error al crear el socket servidor");
        return EXIT_FAILURE;
    }

    printf("Servidor listo para recibir conexiones\n");
    
    while(1){
        int socketCliente = accept(socketServidor, NULL, NULL);
        
        if (socketCliente == -1){
            perror("Error al aceptar la conexión");
            continue;
        }

        pthread_mutex_lock(&mutexcantidadClientesAtendidos);
        cantidadClientesEnEspera++;
        pthread_mutex_unlock(&mutexcantidadClientesAtendidos);

        printf("Cliente conectado, en espera de ser atendido... (socket %d)\n", socketCliente);

        sem_wait(&semaforoHilosDisponibles);

        send(socketCliente, "1", 2, 0);
        printf("Cliente atendido (socket %d)\n", socketCliente);
        
        pthread_mutex_lock(&mutexcantidadClientesAtendidos);
        int * nuevoSocket = malloc(sizeof(int));
        *nuevoSocket = socketCliente;
        int indiceCliente = cantidadClientesAtendidos;
        cantidadClientesAtendidos++;
        cantidadClientesEnEspera--;
        pthread_mutex_unlock(&mutexcantidadClientesAtendidos);

        if(primerCliente){
            primerCliente = false;
            pthread_create(&threadMonitor, NULL, monitorFinalizacion, (void *)&socketServidor);
            pthread_detach(threadMonitor);
        }
        
        pthread_create(&thread[indiceCliente], NULL, manejarCliente, (void *)nuevoSocket);
        pthread_detach(thread[indiceCliente]);
    }

}

void *manejarCliente(void *arg){
    int socketCliente = *(int *)arg;
    free(arg);
    printf("Cliente conectado\n");
    char buffer[MAX_CADENA];
    char bufferEnvio[MAX_CADENA + 50];
    buffer[0] = 0;

    // Mientras no envie /salir, recibir datos
    while(strcmp(buffer, "salir") != 0){
        int bytesRecibidos = read(socketCliente, buffer, sizeof(buffer) - 1);
        if (bytesRecibidos < 0){
            perror("Error al recibir datos");
            break;
        }
        if(bytesRecibidos == 0){
            printf("Cliente %d desconectado\n", socketCliente);
            break;
        }
        buffer[bytesRecibidos] = '\0';
        printf("CLIENTE: %s\n", buffer);

        sprintf(bufferEnvio, "RECIBIDO: %s", buffer);
        int bytesEnviados = write(socketCliente, bufferEnvio, strlen(bufferEnvio));
        if (bytesEnviados <= 0){
            if(errno == EPIPE){
                printf("Cliente %d desconectado\n", socketCliente);
                break;
            }
            perror("Error al enviar datos");
            break;
        }

        bufferEnvio[bytesEnviados] = '\0';
        printf("SERVIDOR: %s\n", bufferEnvio);
    }

    close(socketCliente);
    printf("Cliente %d desconectado\n", socketCliente);
    pthread_mutex_lock(&mutexcantidadClientesAtendidos);
    cantidadClientesAtendidos--;
    pthread_mutex_unlock(&mutexcantidadClientesAtendidos);
    sem_post(&semaforoHilosDisponibles);
    printf("Cantidad de clientes restantes: %d\n", cantidadClientesAtendidos);
    return NULL;
}

void *monitorFinalizacion(void *arg){
    int socketServidor = *(int *)arg;
    while(1){
        sleep(ESPERA_MONITOR_FINALIZACION);
        pthread_mutex_lock(&mutexcantidadClientesAtendidos);
        if(cantidadClientesAtendidos == 0 && cantidadClientesEnEspera == 0){
            pthread_mutex_unlock(&mutexcantidadClientesAtendidos);
            printf("Todos los clientes han terminado...\n");
            close(socketServidor);
            printf("Servidor finalizado correctamente\n");
            sem_destroy(&semaforoHilosDisponibles);
            exit(EXIT_SUCCESS);
        }
        pthread_mutex_unlock(&mutexcantidadClientesAtendidos);
    }
    return NULL;
}

void cerrarServidorSigInt(int sig){
    printf("\nServidor finalizado por senial SIGINT%d\n", sig);
    close(socketServidor);
    sem_destroy(&semaforoHilosDisponibles);
    exit(EXIT_SUCCESS);
}