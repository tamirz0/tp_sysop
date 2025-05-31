#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "../../lib_producto/producto.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#define PUERTO 5000
#define MAX_CLIENTES 2
#define MAX_ENCOLADOS 10
#define IP_SERVIDOR "127.0.0.1"
#define MAX_CADENA 1024
#define TIMEOUT_PRIMERA_LECTURA 1
#define ESPERA_MONITOR_FINALIZACION 1
#define COMANDO_SALIR "/salir"

typedef enum codigo_comunicacion{
    ERROR_ENVIAR_MENSAJE,
    ERROR_ENVIAR_CONEXION_CERRADA,
    ERROR_LEER_CONEXION_CERRADA,
    ERROR_LEER_MENSAJE,
    ENVIAR_CORRECTO,
    LEER_CORRECTO
}CodigoComunicacion;

typedef struct{
    char buffer[MAX_CADENA];
    int socket;
    CodigoComunicacion codigo;
}Mensaje;

int crearSocketServidor();
int crearSocketCliente();
bool enviarMensaje(Mensaje *mensaje);
bool leerMensaje(Mensaje *mensaje);



#endif // COMUNICACION_H