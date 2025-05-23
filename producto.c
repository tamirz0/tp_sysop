#include <producto.h>

int leerCsvProductos(char * path, Producto * productos){
    FILE * pf = fopen(path, 'r+');

    if(pf == NULL){
        return ERR_ARCH;
    }

    int cantReg, i = 0;
    fscanf(pf, "%d", &cantReg);
    
    productos = malloc(cantReg * sizeof(Producto));

    if(productos == NULL){
        fclose(pf);
        return ERR_MEM;
    }

    while(!feof(pf)){
        fscanf(pf, "%d;%49[^;];%f;%d;%29[^\n]\n", &productos[i].id, productos[i].nombre, &productos[i].precio, &productos[i].stock, productos[i].categoria);
        i++;
    }
    
    fclose(pf);
    return i;
}