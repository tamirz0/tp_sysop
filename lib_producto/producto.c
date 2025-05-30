#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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

int serializar(Producto * elemento, char * cadena){
    if(elemento == NULL){
        return ERR_PARAM;
    }

    sprintf(cadena, "%4d|%s|%.2lf|%03d|%s", elemento->id, elemento->nombre, elemento->precio, elemento->stock, elemento->categoria);
    // 4 + 49 + 7 + 3 + 29 + 1 = 93
    return OK;
}

int deSerializar(Producto * elemento, char * cadena){
    if(elemento == NULL){
        return ERR_PARAM;
    }

    sscanf(cadena, "%d|%[^|]|%lf|%d|%[^\n]", &elemento->id, elemento->nombre, &elemento->precio, &elemento->stock, elemento->categoria);
    return OK;
}

FILE * abrirArchivo(char * path){
    FILE * pf = fopen(path, "a+");

    if(pf == NULL){
        return NULL;
    }

    rewind(pf);
    return pf;
}

int buscarId(FILE * pf, int id){
    Producto producto;
    bool encontrado = false;
    int i = 0;
    rewind(pf);
    while(!feof(pf) && !encontrado){
        fscanf(pf, "%d;%[^;];%lf;%d;%[^\n]", &producto.id, producto.nombre, &producto.precio, &producto.stock, producto.categoria);
        if(producto.id == id){
            encontrado = true;
        }
        else{
            i++;
        }
    }

    return encontrado ? i : -1;
}

int buscarProducto(FILE * pf, Producto * producto){
    int i = buscarId(pf, producto->id);
    if(i == -1){
        return ERR_PARAM;
    }

    // BUSCO EN EL CSV
    rewind(pf); // Volvemos al inicio del archivo
    
    // Saltamos las líneas hasta llegar a la deseada
    for(int j = 0; j < i; j++) {
        if(fscanf(pf, "%*[^\n]\n") == EOF) {
            return ERR_PARAM;
        }
    }
    
    // Leemos la línea que nos interesa
    if(fscanf(pf, "%d;%[^;];%lf;%d;%[^\n]\n", 
        &producto->id, 
        producto->nombre, 
        &producto->precio, 
        &producto->stock, 
        producto->categoria) == 5) {
        return OK;
    }
    
    return ERR_PARAM;
}

int agregarProducto(FILE * pf, Producto * producto){
    if(pf == NULL || producto == NULL){
        return ERR_PARAM;
    }
    
    if(buscarId(pf, producto->id) != -1){
        return ERR_DUPLICADO;
    }

    fseek(pf, -1, SEEK_END);

    char c;
    fscanf(pf, "%c", &c);
    if(c != '\n'){
        fprintf(pf, "\n");
    }

    fprintf(pf, "%d;%s;%.2lf;%d;%s", producto->id, producto->nombre, producto->precio, producto->stock, producto->categoria);
    return OK;
}

int eliminarProducto(FILE * pf, Producto * producto, char * path){
    if(pf == NULL || producto == NULL){
        return ERR_PARAM;
    }
    int i = buscarId(pf, producto->id);
    if(i == -1){
        return ERR_PARAM;
    }

    rewind(pf);

    char * archAux = "archAux.csv";
    FILE * pfAux = fopen(archAux, "w");
    if(pfAux == NULL){
        return ERR_ARCH;
    }

    Producto productoAux;
    while(fscanf(pf, "%d;%[^;];%lf;%d;%[^\n]\n", &productoAux.id, productoAux.nombre, &productoAux.precio, &productoAux.stock, productoAux.categoria) == 5){
        if(productoAux.id != producto->id){
            fprintf(pfAux, "%d;%s;%.2lf;%d;%s\n", productoAux.id, productoAux.nombre, productoAux.precio, productoAux.stock, productoAux.categoria);
        }
    }
    
    cerrarArchivo(pfAux);
    cerrarArchivo(pf);

    remove(path);
    rename(archAux, path);

    pf = abrirArchivo(path);
    if(pf == NULL){
        return ERR_ARCH;
    }

    return OK;
}

void cerrarArchivo(FILE * pf){
    fclose(pf);
    pf = NULL;
}