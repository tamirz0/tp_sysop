CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread -lrt

SRCS = main.c producto.c paralelismo.c
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