#include "comunicacion.h"

int crearSocketServidor(){
    struct sockaddr_in serverConfig;
    memset(&serverConfig, 0, sizeof(serverConfig));

    serverConfig.sin_family = AF_INET;
    serverConfig.sin_addr.s_addr = htonl(INADDR_ANY);
    serverConfig.sin_port = htons(PUERTO);

    int socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor == -1){
        return -1;
    }

    // Permitir reutilización del puerto
    int opt = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(socketServidor);
        return -1;
    }

    if (bind(socketServidor, (struct sockaddr *)&serverConfig, sizeof(serverConfig)) == -1){
        close(socketServidor);
        return -1;
    }

    if (listen(socketServidor, MAX_ENCOLADOS) == -1){
        close(socketServidor);
        return -1;
    }

    return socketServidor;
}

int crearSocketCliente(){
    struct sockaddr_in socketConfig;
    memset(&socketConfig, 0, sizeof(socketConfig));

    socketConfig.sin_family = AF_INET;
    socketConfig.sin_port = htons(PUERTO);
    inet_pton(AF_INET, IP_SERVIDOR, &socketConfig.sin_addr);

    int socketCliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCliente < 0){
        return -1;
    }

    if (connect(socketCliente, (struct sockaddr *)&socketConfig, sizeof(socketConfig)) < 0){
        close(socketCliente);
        return -1;
    }

    return socketCliente;
}

bool enviarMensaje(Mensaje *mensaje){
    int bytesEnviados = write(mensaje->socket, mensaje->buffer, strlen(mensaje->buffer));
    if(bytesEnviados <= 0){
        if(errno == EPIPE){
            mensaje->codigo = ERROR_ENVIAR_CONEXION_CERRADA;
        }else{
            mensaje->codigo = ERROR_ENVIAR_MENSAJE;
        }
        return false;
    }
    mensaje->codigo = ENVIAR_CORRECTO;
    return true;
}

bool leerMensaje(Mensaje *mensaje){
    int bytesRecibidos = read(mensaje->socket, mensaje->buffer, sizeof(mensaje->buffer));
    if(bytesRecibidos == 0){
        mensaje->codigo = ERROR_LEER_CONEXION_CERRADA;
        return false;
    }
    if(bytesRecibidos < 0){
        mensaje->codigo = ERROR_LEER_MENSAJE;
        return false;
    }
    mensaje->buffer[bytesRecibidos] = '\0';
    mensaje->codigo = LEER_CORRECTO;
    return true;
}

int obtenerComando(const char * comando){
    if(strcmp(comando, COMANDO_AGREGAR) == 0){
        return AGREGAR;
    }
    if(strcmp(comando, COMANDO_MODIFICAR) == 0){
        return MODIFICAR;
    }
    if(strcmp(comando, COMANDO_ELIMINAR) == 0){
        return ELIMINAR;
    }
    if(strcmp(comando, COMANDO_BUSCAR) == 0){
        return BUSCAR;
    }
    if(strcmp(comando, COMANDO_SALIR) == 0){
        return SALIR;
    }
    return DESCONOCIDO;
}