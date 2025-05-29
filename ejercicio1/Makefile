CC = gcc
CFLAGS = -Wall -Wextra -g -I../lib_producto
LDFLAGS = -pthread -lrt

SRCS = main.c ../lib_producto/producto.c paralelismo.c
OBJS = $(SRCS:.c=.o)
TARGET = programa

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run 