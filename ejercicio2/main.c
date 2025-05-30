#include "../lib_producto/producto.h"
#include <string.h>
int main(){
    /*Producto producto;
    FILE * pf = abrirArchivo("test.csv");
    if(pf == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }
    
    producto.id = 9999;
    int i = buscarProducto(pf, &producto);
    if(i == ERR_PARAM){
        printf("Error al buscar el producto\n");
        return 1;
    }
    mostrarProducto(&producto);

    Producto producto2;
    producto2.id = 9999;
    strcpy(producto2.nombre, "Producto 2");
    producto2.precio = 20.0;
    producto2.stock = 20;
    strcpy(producto2.categoria, "Categoria 2");

    agregarProducto(pf, &producto2);



    cerrarArchivo(pf);*/

    // Test serializar y deSerializar
    /*
    Producto producto, productoDeSerializado;
    producto.id = 9999;
    strcpy(producto.nombre, "Producto 2");
    producto.precio = 20.0;
    producto.stock = 20;
    strcpy(producto.categoria, "Categoria 2");


    char cadena[TAM_CADENA_SERIALIZADA];
    serializar(&producto, cadena);
    printf("Cadena serializada: \n%s\n", cadena);

    deSerializar(&productoDeSerializado, cadena);
    mostrarProducto(&productoDeSerializado);
    */

    // Test eliminarProducto
    Producto producto;
    producto.id = 1010;
    FILE * pf = abrirArchivo("test.csv");
    if(pf == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }
    
    int i = eliminarProducto(pf, &producto, "test.csv");
    if(i == ERR_PARAM){
        printf("Error al eliminar el producto\n");
        return 1;
    }


    Producto producto2;
    producto2.id = 1010;
    strcpy(producto2.nombre, "Producto 2");
    producto2.precio = 20.0;
    producto2.stock = 20;
    strcpy(producto2.categoria, "Categoria 2");

    agregarProducto(pf, &producto2);

    cerrarArchivo(pf);
}

/*
    Para ejecutar el programa:
    gcc -o programa main.c ../lib_producto/producto.c
    ./programa
*/
