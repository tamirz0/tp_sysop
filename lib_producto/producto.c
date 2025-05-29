#include "producto.h"

int leerCsvProductos(char * path, vectorProductos * vector){
    FILE * pf = fopen(path, "r+");

    if(pf == NULL){
        return ERR_ARCH;
    }

    int cantReg, i = 0;
    fscanf(pf, "%d", &cantReg);
    
    if(cantReg > MAX_PRODUCTOS){
        fclose(pf);
        return ERR_PARAM;
    }

    while(!feof(pf) && i < cantReg){
        fscanf(pf, "%d;%49[^;];%lf;%d;%29[^\n]\n", &vector->productos[i].id, vector->productos[i].nombre, &vector->productos[i].precio, &vector->productos[i].stock, vector->productos[i].categoria);
        i++;
    }
    
    fclose(pf);
    
    vector->cant = i;
    return OK;
}

int ejecutarTarea(Producto * elemento, accion accion, void * aux){
    if(accion == NULL || elemento == NULL){
        return ERR_PARAM;
    }

    accion(elemento, aux);
    return OK;
}


void mostrarProducto(Producto * elemento){
    if(elemento == NULL){
        return;
    }

    printf("ID: %d, Nombre: %s, Precio: %.2f, Stock: %d, Categoria: %s\n", elemento->id, elemento->nombre, elemento->precio, elemento->stock, elemento->categoria);
}

void mostrarVectorProductos(vectorProductos * vector){
    if(vector == NULL){
        return;
    }

    for(int i = 0; i < vector->cant; i++){
        mostrarProducto(&vector->productos[i]);
    }
}

void mostrarVector(Producto * vector, int cant){
    if(vector == NULL){
        return;
    }

    for(int i = 0; i < cant; i++){
        mostrarProducto(&vector[i]);
    }
}