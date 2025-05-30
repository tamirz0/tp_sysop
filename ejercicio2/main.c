#include "../lib_producto/producto.h"
#include <stdio.h>
#include <string.h>

void testSerializacion(){
    printf("\n=== Test de Serialización ===\n");
    Producto producto;
    producto.id = 1001;
    strcpy(producto.nombre, "Laptop Gamer");
    producto.precio = 1299.99;
    producto.stock = 10;
    strcpy(producto.categoria, "Electronica");

    char cadena[TAM_CADENA_SERIALIZADA];
    int r = serializar(&producto, cadena);
    printf("Serialización: %s\n", r == OK ? "OK" : "ERROR");
    printf("Cadena serializada: %s\n", cadena);

    Producto producto2;
    r = deSerializar(&producto2, cadena);
    printf("Deserialización: %s\n", r == OK ? "OK" : "ERROR");
    printf("Producto deserializado: ID=%d, Nombre=%s, Precio=%.2f, Stock=%d, Categoria=%s\n",
           producto2.id, producto2.nombre, producto2.precio, producto2.stock, producto2.categoria);
}

void testArchivo(){
    printf("\n=== Test de Archivo ===\n");
    Archivo arch;
    int r = abrirArchivo(&arch, "test.csv");
    printf("Abrir archivo: %s\n", r == OK ? "OK" : "ERROR");

    // Test agregar producto
    Producto producto;
    producto.id = 1011;
    strcpy(producto.nombre, "Nuevo Producto");
    producto.precio = 199.99;
    producto.stock = 5;
    strcpy(producto.categoria, "Accesorios");

    r = agregarProducto(&arch, &producto);
    printf("Agregar producto: %s\n", r == OK ? "OK" : "ERROR");

    // Test buscar producto
    r = buscarProducto(&arch, &producto);
    printf("Buscar producto: %s\n", r == OK ? "OK" : "ERROR");
    if(r == OK){
        printf("Producto encontrado: ID=%d, Nombre=%s\n", producto.id, producto.nombre);
    }

    // Test modificar producto
    producto.precio = 299.99;
    producto.stock = 15;
    r = modificarProducto(&arch, &producto);
    printf("Modificar producto: %s\n", r == OK ? "OK" : "ERROR");

    // Verificar modificación
    r = buscarProducto(&arch, &producto);
    if(r == OK){
        printf("Producto modificado: ID=%d, Precio=%.2f, Stock=%d\n", 
               producto.id, producto.precio, producto.stock);
    }

    // Test eliminar producto
    r = eliminarProducto(&arch, &producto);
    printf("Eliminar producto: %s\n", r == OK ? "OK" : "ERROR");

    // Verificar eliminación
    r = buscarProducto(&arch, &producto);
    printf("Buscar producto eliminado: %s\n", r == ERR_PARAM ? "OK (no encontrado)" : "ERROR");

    cerrarArchivo(&arch);
}

int main(){
    printf("=== Iniciando tests del ejercicio 2 ===\n");
    
    testSerializacion();
    testArchivo();
    
    printf("\n=== Tests completados ===\n");
    return 0;
}

/*
    Para ejecutar el programa:
    gcc -o programa main.c ../lib_producto/producto.c
    ./programa
*/

