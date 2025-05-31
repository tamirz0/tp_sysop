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
    
    char buffer[MAX_CADENA];
    buffer[0] = 0;

    read(socketCliente, buffer, sizeof(buffer));
    buffer[strlen(buffer)] = 0;
    printf("Turno obtenido\nComenzando a enviar mensajes...\n");
    
    while(strcmp(buffer, "salir") != 0){
        printf("Ingrese un mensaje (o salir): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if(strcmp(buffer, "salir") == 0){
            printf("Saliendo del cliente\n");
            int bytesEnviados = write(socketCliente, buffer, strlen(buffer));
            if (bytesEnviados <= 0){
                perror("Error al enviar mensaje de salida");
            }
            printf("Conexión cerrada\n");
            close(socketCliente);
            return EXIT_SUCCESS;
        }

        int bytesEnviados = write(socketCliente, buffer, strlen(buffer));
        if (bytesEnviados <= 0){
            perror("Error al enviar datos");
            close(socketCliente);
            return EXIT_FAILURE;
        }
        buffer[bytesEnviados] = '\0';
        printf("CLIENTE: %s\n", buffer);

        int bytesRecibidos = read(socketCliente, buffer, sizeof(buffer) - 1);
        if(bytesRecibidos == 0){
            printf("Conexión cerrada\n");
            close(socketCliente);
            return EXIT_SUCCESS;
        }
        
        if (bytesRecibidos < 0){
            perror("Error al recibir datos");
            close(socketCliente);
            return EXIT_FAILURE;
        }
        buffer[bytesRecibidos] = '\0';
        printf("SERVIDOR: %s\n", buffer);
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