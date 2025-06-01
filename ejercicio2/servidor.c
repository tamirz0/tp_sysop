#include "lib_comunicacion/comunicacion.h"
#include <stdio.h>
#include <stdlib.h>

/*
    El servidor arranca y escucha conexiones en un puerto fijo.
    Un cliente se conecta → el servidor lanza un hilo nuevo para manejarlo.
    El servidor muestra un menú textual al cliente y espera un comando.
    El servidor procesa el comando y responde con un mensaje de confirmación o error.
        En caso de ser una consulta, el mensaje de confirmación es retornar la información como un string.
        En caso de ser un comando permitido que no sea salida ni consulta, se va a implementar el uso de 
            un semáforo para evitar que se produzcan condiciones de carrera si es solicitado un mismo producto 
            por 2 clientes o más al mismo tiempo.
    Al recibir /salir, el hilo del cliente se cierra.
    Cuando se ejecuta un CTRL + C, finaliza el programa.
 */

/*
    EJECUCION
    make
    ./servidor <archivo.csv> ### En una terminal
    ./cliente ### En diferentes terminales

    MONITORIZACION
        pidof servidor
        pidof cliente
    SOCKETS - CONEXIONES 
        watch -n 1 "sudo lsof -iTCP:5000" ### ver las conexiones asociadas al puerto 5000 *
        watch -n 1 "ls -l /proc/<PID>/fd | grep socket" ### ver los sockets asociados a el proceso del servidor *
        netstat -tulnp | grep 5000

    HILOS (como maximo seran 5 (el principal, el monitor y los 3 clientes)) 
        ps -L -p <PID> ### ver los hilos del proceso
        htop ### ver los hilos del proceso *
        top -H -p <PID> ### ver los hilos del proceso *
*/

/// VARIABLES GLOBALES ///
Archivo archivoCsv;
pthread_mutex_t mutexArchivoCsv = PTHREAD_MUTEX_INITIALIZER; // Mutex para el archivo CSV

int socketServidor;
sem_t semaforoHilosDisponibles; // Semáforo para controlar la cantidad de hilos disponibles

int cantidadClientesAtendidos = 0; // Cantidad de clientes conectados (atendidos)
int cantidadClientesEnEspera = 0; // Cantidad de clientes en espera de ser atendidos
pthread_mutex_t mutexcantidadClientesAtendidos = PTHREAD_MUTEX_INITIALIZER; // Mutex para la cantidad de clientes

/*
    Clientes en espera necesita ser considerado para la finalizacion del servidor.
    Ya que si se finaliza un cliente atendido, y justo antes de que se atienda el siguiente cliente en espera,
    el monitorFinalizacion se ejecuta, entonces se habria finalizado el servidor cuando no deberia.
*/


void * manejarCliente(void * arg);
void * monitorFinalizacion(void * arg);
void cerrarServidorSigInt(int sig);

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Error: cantidad de argumentos incorrecta\n");
        return EXIT_FAILURE;
    }
    // Manejo de señales de finalizacion
    signal(SIGINT, cerrarServidorSigInt); // CTRL + C
    signal(SIGPIPE, SIG_IGN); // Cliente desconectado
    
    pthread_t thread[MAX_CLIENTES];
    pthread_t threadMonitor;
    bool primerCliente = true; // Variable de control para iniciar el monitor de finalizacion

    if(abrirArchivo(&archivoCsv, argv[1]) != OK){
        printf("Error: no se pudo abrir el archivo CSV\n");
        return EXIT_FAILURE;
    }

    socketServidor = crearSocketServidor();
    if (socketServidor == -1){
        perror("Error al crear el socket servidor");
        return EXIT_FAILURE;
    }
    
    sem_init(&semaforoHilosDisponibles, 0, MAX_CLIENTES);
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
        
        sem_wait(&semaforoHilosDisponibles); // P()
        send(socketCliente, "1", 2, 0); // Envio mensaje "1" para desbloquear al cliente
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

void * manejarCliente(void * arg){
    Mensaje mensaje;
    mensaje.buffer[0] = 0;
    mensaje.socket = *(int *)arg;
    free(arg);
    Producto producto;
    int resultado;

    while(1){
        if(!leerMensaje(&mensaje)){
            printf("Error al recibir mensaje del cliente %d - Fin de la conexion\n", mensaje.socket);
            break;
        }

        int comando = obtenerComando(mensaje.buffer);
        if(comando == SALIR){
            printf("Cliente %d saliendo\n", mensaje.socket);
            break;
        }
        
        switch(comando){
            case AGREGAR:
                if(!leerMensaje(&mensaje)){
                    printf("Error al recibir mensaje\n");
                    break;
                }

                deSerializar(&producto, mensaje.buffer);
                pthread_mutex_lock(&mutexArchivoCsv);
                resultado = agregarProducto(&archivoCsv, &producto);
                pthread_mutex_unlock(&mutexArchivoCsv);

                if(resultado == OK){
                    sprintf(mensaje.buffer, "%s", OPERACION_EXITO);
                    printf("PRODUCTO AGREGADO %d\n", producto.id);
                }
                else{
                    sprintf(mensaje.buffer, "%s", OPERACION_ERROR);
                }

                if(!enviarMensaje(&mensaje)){ // Envio mensaje de confirmacion de operacion
                    printf("Error al enviar mensaje\n");
                    break;
                }
            break;
            case MODIFICAR:
                if(!leerMensaje(&mensaje)){
                    printf("Error al recibir mensaje\n");
                    break;
                }

                deSerializar(&producto, mensaje.buffer);
                pthread_mutex_lock(&mutexArchivoCsv);
                resultado = modificarProducto(&archivoCsv, &producto);
                pthread_mutex_unlock(&mutexArchivoCsv);

                if(resultado == OK){
                    sprintf(mensaje.buffer, "%s", OPERACION_EXITO);
                    printf("PRODUCTO MODIFICADO %d\n", producto.id);
                }
                else{
                    sprintf(mensaje.buffer, "%s", OPERACION_ERROR);
                }
                
                if(!enviarMensaje(&mensaje)){
                    printf("Error al enviar mensaje\n");
                    break;
                }
            break;
            case ELIMINAR:
                if(!leerMensaje(&mensaje)){
                    printf("Error al recibir mensaje\n");
                    break;
                }

                deSerializar(&producto, mensaje.buffer);
                pthread_mutex_lock(&mutexArchivoCsv);
                resultado = eliminarProducto(&archivoCsv, &producto);
                pthread_mutex_unlock(&mutexArchivoCsv);

                if(resultado == OK){
                    sprintf(mensaje.buffer, "%s", OPERACION_EXITO);
                    printf("PRODUCTO ELIMINADO %d\n", producto.id);
                }
                else{
                    sprintf(mensaje.buffer, "%s", OPERACION_ERROR);
                }

                if(!enviarMensaje(&mensaje)){
                    printf("Error al enviar mensaje\n");
                    break;
                }
            break;
            case BUSCAR:
                if(!leerMensaje(&mensaje)){
                    printf("Error al recibir mensaje\n");
                    break;
                }

                deSerializar(&producto, mensaje.buffer);
                pthread_mutex_lock(&mutexArchivoCsv);
                resultado = buscarProducto(&archivoCsv, &producto);
                pthread_mutex_unlock(&mutexArchivoCsv);

                if(resultado == OK){
                    serializar(&producto, mensaje.buffer);
                    printf("PRODUCTO BUSCADO %d\n", producto.id);
                }
                else{
                    sprintf(mensaje.buffer, "%s", OPERACION_ERROR);
                }

                if(!enviarMensaje(&mensaje)){
                    printf("Error al enviar mensaje\n");
                    break;
                }
            break;
            default:
                break;
        }
    }

    close(mensaje.socket);

    pthread_mutex_lock(&mutexcantidadClientesAtendidos);
    cantidadClientesAtendidos--;
    pthread_mutex_unlock(&mutexcantidadClientesAtendidos);
    sem_post(&semaforoHilosDisponibles); // V()

    return NULL;
}

void *monitorFinalizacion(void *arg){
    int socket = *(int *)arg;
    while(1){
        sleep(ESPERA_MONITOR_FINALIZACION);

        pthread_mutex_lock(&mutexcantidadClientesAtendidos);
        if(cantidadClientesAtendidos == 0 && cantidadClientesEnEspera == 0){
            pthread_mutex_unlock(&mutexcantidadClientesAtendidos);

            printf("Todos los clientes han terminado...\n");
            close(socket);
            sem_destroy(&semaforoHilosDisponibles);
            cerrarArchivo(&archivoCsv);
            printf("Servidor finalizado correctamente\n");
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
    cerrarArchivo(&archivoCsv);
    printf("Servidor finalizado correctamente\n");
    exit(EXIT_SUCCESS);
}