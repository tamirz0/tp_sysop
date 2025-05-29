#include "../lib_producto/producto.h"
#include <string.h>
int main(){
    Producto producto;
    FILE * pf = abrirArchivo("test.csv");
    if(pf == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }
    
    producto.id = 1005;
    int i = buscarProducto(pf, &producto);
    if(i == ERR_PARAM){
        printf("Error al buscar el producto\n");
        return 1;
    }
    mostrarProducto(&producto);

    fclose(pf);
}

/*
    Para ejecutar el programa:
    gcc -o programa ejercicio2/main.c ../lib_producto/producto.c
    ./programa
*/
