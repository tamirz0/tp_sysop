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

int abrirArchivo(Archivo * arch, char * path){
    if(arch == NULL){
        return ERR_PARAM;
    }

    arch->pf = fopen(path, "a+");
    if(arch->pf == NULL){
        return ERR_ARCH;
    }

    
    strcpy(arch->path, path);
    return OK;
}

int buscarId(Archivo * arch, int id){
    Producto producto;
    bool encontrado = false;
    int i = 0;
    rewind(arch->pf);
    while(!feof(arch->pf) && !encontrado){
        fscanf(arch->pf, "%d;%[^;];%lf;%d;%[^\n]", &producto.id, producto.nombre, &producto.precio, &producto.stock, producto.categoria);
        if(producto.id == id){
            encontrado = true;
        }
        else{
            i++;
        }
    }

    return encontrado ? i : -1;
}

int buscarProducto(Archivo * arch, Producto * producto){
    int i = buscarId(arch, producto->id);
    if(i == -1){
        return ERR_PARAM;
    }

    rewind(arch->pf);
    
    for(int j = 0; j < i; j++) {
        if(fscanf(arch->pf, "%*[^\n]\n") == EOF) {
            return ERR_PARAM;
        }
    }
    
    if(fscanf(arch->pf, "%d;%[^;];%lf;%d;%[^\n]\n", 
        &producto->id, 
        producto->nombre, 
        &producto->precio, 
        &producto->stock, 
        producto->categoria) == 5) {
        return OK;
    }
    
    return ERR_PARAM;
}

int agregarProducto(Archivo * arch, Producto * producto){
    if(arch == NULL || producto == NULL){
        return ERR_PARAM;
    }
    
    if(buscarId(arch, producto->id) != -1){
        return ERR_DUPLICADO;
    }

    fseek(arch->pf, -1, SEEK_END);

    char c;
    fscanf(arch->pf, "%c", &c);
    if(c != '\n'){
        fprintf(arch->pf, "\n");
    }

    fprintf(arch->pf, "%d;%s;%.2lf;%d;%s", producto->id, producto->nombre, producto->precio, producto->stock, producto->categoria);
    return OK;
}

int eliminarProducto(Archivo * arch, Producto * producto){
    if(arch == NULL || producto == NULL){
        return ERR_PARAM;
    }
    int i = buscarId(arch, producto->id);
    if(i == -1){
        return ERR_NO_ENCONTRADO;
    }

    rewind(arch->pf);

    char * archAux = "archAux.csv";
    FILE * pfAux = fopen(archAux, "w");
    if(pfAux == NULL){
        return ERR_ARCH;
    }

    Producto productoAux;
    while(fscanf(arch->pf, "%d;%[^;];%lf;%d;%[^\n]\n", &productoAux.id, productoAux.nombre, &productoAux.precio, &productoAux.stock, productoAux.categoria) == 5){
        if(productoAux.id != producto->id){
            fprintf(pfAux, "%d;%s;%.2lf;%d;%s\n", productoAux.id, productoAux.nombre, productoAux.precio, productoAux.stock, productoAux.categoria);
        }
    }
    
    fclose(pfAux);
    cerrarArchivo(arch);

    remove(arch->path);
    rename(archAux, arch->path);

    arch->pf = fopen(arch->path, "a+");
    if(arch->pf == NULL){
        return ERR_ARCH;
    }

    return OK;
}

int modificarProducto(Archivo * arch, Producto * producto){
    if(arch == NULL || producto == NULL){
        return ERR_PARAM;
    }

    if(eliminarProducto(arch, producto) == OK){
        agregarProducto(arch, producto);
    }
    else{
        return ERR_ARCH;
    }

    return OK;
}

void cerrarArchivo(Archivo * arch){
    if(arch != NULL){
        fclose(arch->pf);
        arch->pf = NULL;
    }
}

Producto leerProducto(){
    Producto producto;
    char buffer[TAM_CADENA_SERIALIZADA];
    
    printf("Ingrese el ID del producto: ");
    scanf("%d", &producto.id);
    getchar(); // Consumir el \n del scanf anterior
    
    printf("Ingrese el nombre del producto: ");
    fgets(buffer, TAM_NOMBRE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    strcpy(producto.nombre, buffer);
    
    printf("Ingrese el precio del producto: ");
    scanf("%lf", &producto.precio);
    getchar(); // Consumir el \n del scanf anterior
    
    printf("Ingrese el stock del producto: ");
    scanf("%d", &producto.stock);
    getchar(); // Consumir el \n del scanf anterior
    
    printf("Ingrese la categoria del producto: ");
    fgets(buffer, TAM_CATEGORIA, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    strcpy(producto.categoria, buffer);
    
    return producto;
}