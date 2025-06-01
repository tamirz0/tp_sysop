CC = gcc
CFLAGS = -Wall -Wextra -g -I../lib_producto -I./lib_comunicacion
LDFLAGS = -pthread

SRCS_CLIENTE = cliente.c lib_comunicacion/comunicacion.c ../lib_producto/producto.c
SRCS_SERVIDOR = servidor.c lib_comunicacion/comunicacion.c ../lib_producto/producto.c

OBJS_CLIENTE = $(SRCS_CLIENTE:.c=.o)
OBJS_SERVIDOR = $(SRCS_SERVIDOR:.c=.o)

TARGET_CLIENTE = cliente
TARGET_SERVIDOR = servidor

all: $(TARGET_CLIENTE) $(TARGET_SERVIDOR)

$(TARGET_CLIENTE): $(OBJS_CLIENTE)
	$(CC) $(OBJS_CLIENTE) -o $(TARGET_CLIENTE) $(LDFLAGS)

$(TARGET_SERVIDOR): $(OBJS_SERVIDOR)
	$(CC) $(OBJS_SERVIDOR) -o $(TARGET_SERVIDOR) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_CLIENTE) $(OBJS_SERVIDOR) $(TARGET_CLIENTE) $(TARGET_SERVIDOR)

.PHONY: all clean 