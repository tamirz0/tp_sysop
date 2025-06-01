#include "lib_comunicacion/comunicacion.h"

int socketCliente;

void cerrarClienteSigInt(int sig);

int main(){
    signal(SIGINT, cerrarClienteSigInt);
    
    Mensaje mensaje;
    Producto producto;
    int comando = DESCONOCIDO;

    mensaje.socket = crearSocketCliente();
    if (mensaje.socket == -1){
        perror("Error al crear el socket cliente");
        return EXIT_FAILURE;
    }

    printf("Conectado al servidor\nEsperando turno...\n");
    if(!leerMensaje(&mensaje)){
        printf("Error al obtener turno\n");
        close(mensaje.socket);
        return EXIT_FAILURE;
    }

    printf("Turno obtenido\nComenzando a enviar mensajes...\n");
    

    while(comando != SALIR){
        do{
            printf("Ingrese un comando (o %s): ", COMANDO_SALIR);
            fgets(mensaje.buffer, sizeof(mensaje.buffer), stdin);
            mensaje.buffer[strcspn(mensaje.buffer, "\n")] = 0;
            comando = obtenerComando(mensaje.buffer);
            if(comando == DESCONOCIDO){
                printf("Comando desconocido\n");
                continue;
            }
        }while(comando == DESCONOCIDO);

        if(!enviarMensaje(&mensaje)){ //envio el comando al servidor
            printf("Error al enviar mensaje\n");
            break;
        }

        switch(comando){
            case AGREGAR:
                producto = leerProducto();
                serializar(&producto, mensaje.buffer);
                enviarMensaje(&mensaje);
                leerMensaje(&mensaje);
                if(strcmp(mensaje.buffer, OPERACION_EXITO) == 0){
                    printf("Producto agregado correctamente\n");
                }else if(strcmp(mensaje.buffer, OPERACION_ERROR) == 0){
                    printf("Error al agregar producto\n");
                }else{
                    printf("Se cerro la conexion con el servidor\n");
                    close(socketCliente);
                    return EXIT_FAILURE;
                }
                break;
            case MODIFICAR:
                producto = leerProducto();
                serializar(&producto, mensaje.buffer);
                enviarMensaje(&mensaje);
                leerMensaje(&mensaje);
                if(strcmp(mensaje.buffer, OPERACION_EXITO) == 0){
                    printf("Producto modificado correctamente\n");
                }else if(strcmp(mensaje.buffer, OPERACION_ERROR) == 0){
                    printf("Error al modificar producto\n");
                }else{
                    printf("Se cerro la conexion con el servidor\n");
                    close(socketCliente);
                    return EXIT_FAILURE;
                }
                break;
            case ELIMINAR:
                printf("Ingrese el id del producto a eliminar: ");
                scanf("%d", &producto.id);
                getchar();
                serializar(&producto, mensaje.buffer);
                enviarMensaje(&mensaje);
                leerMensaje(&mensaje);
                if(strcmp(mensaje.buffer, OPERACION_EXITO) == 0){
                    printf("Producto eliminado correctamente\n");
                }else if(strcmp(mensaje.buffer, OPERACION_ERROR) == 0){
                    printf("Error al eliminar producto\n");
                }else{
                    printf("Se cerro la conexion con el servidor\n");
                    close(socketCliente);
                    return EXIT_FAILURE;
                }
                break;
            case BUSCAR:
                printf("Ingrese el id del producto a buscar: ");
                scanf("%d", &producto.id);
                getchar();
                serializar(&producto, mensaje.buffer);
                enviarMensaje(&mensaje);
                leerMensaje(&mensaje);
                if(strcmp(mensaje.buffer, OPERACION_ERROR) == 0){
                    printf("Producto no encontrado\n");
                }else{
                    deSerializar(&producto, mensaje.buffer);
                    mostrarProducto(&producto);
                }
                break;
            default:
                break;
        }
    }

    printf("Saliendo del cliente\n");
    close(socketCliente);
    printf("Conexión cerrada\n");
    return EXIT_SUCCESS;
}

void cerrarClienteSigInt(int sig){
    printf("\nCliente finalizado por senial SIGINT %d\n", sig);
    close(socketCliente);
    printf("Conexión cerrada\n");
    exit(EXIT_SUCCESS);
}