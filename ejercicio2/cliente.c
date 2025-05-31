#include "lib_comunicacion/comunicacion.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PUERTO 5000
#define MAX_CADENA 1024
#define IP_SERVIDOR "127.0.0.1"

int socketCliente;

void cerrarClienteSigInt(int sig);

int main(){
    signal(SIGINT, cerrarClienteSigInt);
    socketCliente = crearSocketCliente();
    if (socketCliente == -1){
        perror("Error al crear el socket cliente");
        return EXIT_FAILURE;
    }

    printf("Conectado al servidor\nEsperando turno...\n");
    
    Mensaje mensaje;
    mensaje.socket = socketCliente;

    if(!leerMensaje(&mensaje)){
        printf("Error al obtener turno\n");
        close(socketCliente);
        return EXIT_FAILURE;
    }

    printf("Turno obtenido\nComenzando a enviar mensajes...\n");
    
    while(strcmp(mensaje.buffer, COMANDO_SALIR) != 0){
        
        printf("Ingrese un mensaje (o %s): ", COMANDO_SALIR);
        fgets(mensaje.buffer, sizeof(mensaje.buffer), stdin);
        mensaje.buffer[strcspn(mensaje.buffer, "\n")] = 0;

        if(strcmp(mensaje.buffer, COMANDO_SALIR) == 0){
            printf("Saliendo del cliente\n");

            if(!enviarMensaje(&mensaje)){
                printf("Error al enviar mensaje de salida\n");
                close(socketCliente);
                return EXIT_FAILURE;
            }
            
            close(socketCliente);
            printf("Conexión cerrada\n");
            return EXIT_SUCCESS;
        }

        if(!enviarMensaje(&mensaje)){
            printf("Error al enviar mensaje\n");
            close(socketCliente);
            return EXIT_FAILURE;
        }

        if(!leerMensaje(&mensaje)){
            printf("Error al recibir mensaje\n");
            if(mensaje.codigo == ERROR_LEER_CONEXION_CERRADA){
                printf("Conexión cerrada\n");
                close(socketCliente);
                return EXIT_SUCCESS;
            }
            if(mensaje.codigo == ERROR_LEER_MENSAJE){
                printf("Error al recibir mensaje\n");
                close(socketCliente);
                return EXIT_FAILURE;
            }
            close(socketCliente);
            return EXIT_FAILURE;
        }

        printf("SERVIDOR %d recibio: %s\n", socketCliente, mensaje.buffer);
    }

    close(socketCliente);
    printf("Conexión cerrada\n");
    return EXIT_SUCCESS;
}

void cerrarClienteSigInt(int sig){
    printf("\nCliente finalizado por senial SIGINT %d\n", sig);
    close(socketCliente);
    exit(EXIT_SUCCESS);
}