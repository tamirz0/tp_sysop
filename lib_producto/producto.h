#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PRODUCTOS 100
#define TAM_NOMBRE 50
#define TAM_CATEGORIA 30
#define CANT_CAMPOS 5
#define TAM_CADENA_SERIALIZADA 100
#define TAM_PATH 100

#define OK 0
#define ERR_ARCH 100
#define ERR_MEM 200
#define ERR_LECTURA 300
#define ERR_PARAM 400
#define ERR_DUPLICADO 500
#define ERR_NO_ENCONTRADO 600
//#define REGISTROS_CSV 10000

typedef struct{
    int id; // 1000 a 9999
    char nombre[TAM_NOMBRE]; 
    double precio; // 1.00 a 9999.99
    int stock; // 0 a 100
    char categoria[TAM_CATEGORIA];
}Producto;

typedef struct{
    Producto productos[MAX_PRODUCTOS];
    int cant;
}vectorProductos;

typedef struct{
    FILE * pf;
    char path[TAM_PATH];
}Archivo;

typedef void (*accion)(void * e, void * aux);

////////////////////////////////////////////////////////////
////////////////// EJERCICIO 1 ////////////////////////////
//Retorna cantRegistros csv
int leerCsvProductos(char * path, vectorProductos * vector);
//Tareas
int ejecutarTarea(Producto * elemento, accion accion, void * aux);
void mostrarProducto(Producto * elemento);
void mostrarVectorProductos(vectorProductos * vector);
void mostrarVector(Producto * vector, int cant);

////////////////////////////////////////////////////////////
////////////////// EJERCICIO 2 ////////////////////////////
int serializar(Producto * elemento, char * cadena);
int deSerializar(Producto * elemento, char * cadena);

int abrirArchivo(Archivo * arch, char * path);
int buscarProducto(Archivo * arch, Producto * producto);
int agregarProducto(Archivo * arch, Producto * producto);
int eliminarProducto(Archivo * arch, Producto * producto);
int modificarProducto(Archivo * arch, Producto * producto);
void cerrarArchivo(Archivo * arch);

#endif // PRODUCTO_H