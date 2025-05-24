#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <stdio.h>
#include <stdlib.h>


#define TAM_NOMBRE 50
#define TAM_CATEGORIA 30
#define CANT_CAMPOS

#define OK 0
#define ERR_ARCH 100
#define ERR_MEM 200
#define ERR_LECTURA 300
#define ERR_PARAM 400
//#define REGISTROS_CSV 10000

typedef struct{
    int id; // 1000 a 9999
    char nombre[TAM_NOMBRE]; 
    float precio; // 1.00 a 9999.99
    int stock; // 0 a 100
    char categoria[TAM_CATEGORIA];
}Producto;

typedef struct{
    Producto * productos;
    int cant;
}vectorProductos;

typedef void (*accion)(void * e, void * aux);

//Retorna cantRegistros csv
int leerCsvProductos(char * path, vectorProductos * vector);

//Tareas
int ejecutarTarea(Producto * elemento, accion accion, void * aux);

int liberarVectorProductos(vectorProductos * vector);

void mostrarProducto(Producto * elemento);

#endif // PRODUCTO_H