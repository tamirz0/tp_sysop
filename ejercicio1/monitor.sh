#!/bin/bash

while true; do
    clear
    
    PID_PROGRAMA=$(pidof programa)
    
    echo "========== MONITOREO DEL SISTEMA DE PROCESOS ==========="
    echo "Hora: $(date)"
    echo "PID Programa Principal: $PID_PROGRAMA"
    echo "======================================================="

    if [[ -n "$PID_PROGRAMA" ]]; then
        echo ">>> PROCESOS HIJOS:"
        ps -ejH | grep programa || echo "No hay procesos hijos"
        echo ""

        echo ">>> PROCESOS PADRE + HIJOS:"
        ps -eo pid,ppid,comm | grep programa || echo "No hay procesos hijos"
        echo ""

        echo ">>> MEMORIA COMPARTIDA:"
        ls -l /dev/shm/miMemoria* 2>/dev/null || echo "No hay memoria compartida"
        echo ""

        echo ">>> SEMAFOROS:"
        ls -l /dev/shm/sem.* 2>/dev/null || echo "No hay semáforos"
        echo ""

        echo ">>> USO DE MEMORIA DEL PROGRAMA:"
        pmap $PID_PROGRAMA | grep -i "miMemoria" || echo "No se encontró memoria compartida"
        echo ""
    else
        echo "No hay programa corriendo"
    fi

    echo "=========== FIN DEL MONITOR ============"
    sleep 1
done
