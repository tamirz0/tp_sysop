#!/bin/bash

while true; do
    clear  # Limpia la pantalla antes de cada actualización
    
    PUERTO=5000
    PID_SERVIDOR=$(pidof servidor)
    PID_CLIENTE=$(pidof cliente)

    echo "========== MONITOREO DEL SISTEMA SERVIDOR-CLIENTE ==========="
    echo "Hora: $(date)"
    echo "Puerto: $PUERTO"
    echo "Servidor: $PID_SERVIDOR"
    echo "Cliente: $PID_CLIENTE"
    echo "============================================================="

    echo ">>> CONEXIONES EN EL PUERTO $PUERTO:"
    sudo netstat -tulnp | grep "$PUERTO" || echo "No hay conexiones"
    echo ""

    echo ">>> SOCKETS ABIERTOS (lsof):"
    sudo lsof -iTCP:$PUERTO || echo "No hay sockets activos"
    echo ""

    if [[ -n "$PID_SERVIDOR" ]]; then
        echo ">>> SOCKETS DEL SERVIDOR (/proc/$PID_SERVIDOR/fd/):"
        ls -l /proc/$PID_SERVIDOR/fd | grep socket || echo "No hay sockets del servidor"
        echo ""

        echo ">>> HILOS DEL SERVIDOR:"
        ps -L -p $PID_SERVIDOR || echo "No hay hilos del servidor"
        echo ""
    else
        echo "No hay servidor corriendo"
    fi

    if [[ -n "$PID_CLIENTE" ]]; then
        echo ">>> PROCESOS CLIENTES:"
        ps -L -p $PID_CLIENTE || echo "No hay procesos de cliente"
        echo ""
    fi

    echo "=========== FIN DEL MONITOR ============"
    sleep 1  # Espera 1 segundo antes de la siguiente actualización
done