#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_PRODUCTOS 100
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
    double precio; // 1.00 a 9999.99
    int stock; // 0 a 100
    char categoria[TAM_CATEGORIA];
}Producto;

typedef struct{
    Producto productos[MAX_PRODUCTOS];
    int cant;
}vectorProductos;

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


#endif // PRODUCTO_H