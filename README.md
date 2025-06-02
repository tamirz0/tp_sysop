# tp_sysop
Trabajo Practico de la materia Sistemas Operativos. Pensado para ejecutarse en un entorno Linux.

# Diseño Funcional
[diseño_funcional.pdf](https://github.com/user-attachments/files/20559192/diseno_funcional.pdf)

# Descripción
En la presente carpeta se incluyen 3 carpetas:
- **ejercicio1**: Contiene el código del primer ejercicio (Coordinación de Tareas con Procesos y Memoria Compartida).
- **ejercicio2**: Comunicación Cliente-Servidor con Threads y Sockets
- **lib_producto**: Contiene el código de la librería que se utiliza en ambos ejercicios. La misma contiene todo lo referido a la logica de negocio utilizada en ambos ejercicios.

# Compilación
Para compilar el código de cada ejercicio, se debe seguir los siguientes pasos:
1. Abrir una terminal en la carpeta del ejercicio correspondiente.
2. Ejecutar el comando `make` para compilar el código.
3. Si la compilación es exitosa, se generará un ejecutable en la misma carpeta.

# Ejecución
Para ejecutar el código de cada ejercicio, se debe seguir los siguientes pasos:
- **ejercicio1**: Ejecutar el comando `./programa <categoria_a_simular_ventas> <categoria_a_calcular_promedio>`.
- **ejercicio2**: 
 1. Ejecutar el comando `./servidor <nombre_archivo.csv>` en una terminal para iniciar el servidor.
 2. En otra terminal, ejecutar el comando `./cliente` para iniciar cada cliente.

 # Monitoreo
 Para monitorear lo referido a cada ejercicio, se pueden utilizar 2 herramientas:
 - **monitor.sh**: Un script contenido en cada ejercicio que permite monitorear los recursos utilizados por cada uno.

 Pasos para su uso:
 1. Abrir una terminal en la carpeta del ejercicio correspondiente.
 2. ejecutar el comando `chmod +x monitor.sh` para darle permisos de ejecución al script.
 3. Ejecutar el comando `./monitor.sh` para iniciar el monitoreo.
 ***Recuerde ejecutar el script en una terminal con privilegios de superusuario (root) para poder acceder a la información de los procesos.***

 - **comandos individuales**: en el caso del ejercicio 1 en el main y en el ejercicio 2 en el servidor, podra encontrar en comentarios los comandos que se pueden utilizar para monitorear los recursos utilizados por cada programa. 
 Estos son:
    - `ps`
    - `pmap`
    - `htop` (debe instalarlo previamente)
    - `ls`
    - `pidof`
    - `lsof`
    - `netstat`
    - `top`
