#include "producto.h"
#include "paralelismo.h"

/*
    Lee el archivo CSV y guarda todos los productos en un vector Producto[].
    Copia ese vector a un bloque de memoria compartida.
    Crea 4 procesos hijos con fork().
        Los 4 procesos hijos realizan sus respectivas tareas en paralelo
        En caso de utilizarse escritura en el vector se da lugar a un bloqueo de la memoria compartida a través de un semáforo para evitar condición de carrera.
    El usuario puede finalizar la ejecución en cualquier momento
        En caso de que se termine de recorrer el vector, automáticamente se comenzará a recorrer otra vez desde el inicio. 
    El padre espera a que los hijos terminen sus procesos.
    Libera recursos: memoria compartida y semáforos.
    Fin del sistema.
*/

/*
    Comandos para ejecutar el programa:
    make && ./programa categoriaVentas categoriaPromedio
    
    EJ : make && ./programa "Electronica" "Accesorios"
 */

#define NOMBRE_ARCHIVO_CSV "test.csv"
#define NOMBRE_SEMAFORO "miSemaforo"


int main(int argc, char * argv[]){

    if(argc != 3){
        printf("Error: Debe ingresar las categorias de los productos a procesar\n");
        return 1;
    }

    char * categoriaVentas = argv[1];
    char * categoriaPromedio = argv[2];

    // 1 Guardar los productos en un vector
    vectorProductos vector;
    leerCsvProductos(NOMBRE_ARCHIVO_CSV, &vector);

    // 2 Preparar la memoria compartida
    memoriaCompartida memoria;
    memoria.vector = vector;  // Ahora podemos copiar directamente
    memoria.fin = false;
    memoria.semaforo = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0600, 1);

    // 3 Crear el bloque de memoria compartida
    void * ptr = crearBloqueMemoriaCompartida(&memoria, sizeof(memoriaCompartida));
    if(ptr == NULL){
        printf("Error al crear el bloque de memoria compartida\n");
        return 1;
    }

    // 4 Crear los 4 procesos hijos
    for(int i = 0; i < 4; i++){
        int hijo = fork();
        if(hijo == 0){
            if(i == 0){
                tareaHijo1(ptr, categoriaVentas); //Ventas
            }
            else if(i == 1){
                tareaHijo2(ptr); //Aviso Stock alto
            }
            else if(i == 2){
                tareaHijo3(ptr); //Reposicion de stock bajo
            }
            else if(i == 3){
                tareaHijo4(ptr, categoriaPromedio); //Promedio de valor en inventario por producto de la categoria
            }
            exit(0);
        }
    }
    
    printf("Presione Enter para finalizar...\n");
    getchar();

    // Indicar a los hijos que terminen
    memoriaCompartida * mem = (memoriaCompartida *)ptr;
    mem->fin = true;
    
    // Esperar a que todos los hijos terminen
    for(int i = 0; i < 4; i++) {
        wait(NULL);
    }
    printf("Todos los hijos terminaron\n");
    
    // Liberar la memoria compartida
    mostrarVectorProductos(&mem->vector);

    liberarBloqueMemoriaCompartida(ptr, sizeof(memoriaCompartida));
    sem_close(memoria.semaforo);
    sem_unlink(NOMBRE_SEMAFORO);

    return 0;
}
