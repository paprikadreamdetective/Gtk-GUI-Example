# Nombre del ejecutable final
TARGET = steren_manager

# Compilador
CC = gcc

# Flags de compilación
CFLAGS = `pkg-config --cflags gtk+-3.0` -pthread -Wall -g
LDFLAGS = `pkg-config --libs gtk+-3.0` -lrt

# Archivos fuente
SRC = src/main.c src/leer_archivo.c src/list.c src/memoria_compartida.c src/ui.c src/productos.c
# Archivos objeto
OBJ = $(SRC:.c=.o)

# Directorio de include
INCLUDES = -I./include

# Compila todo
all: $(TARGET)

# Regla para compilar el ejecutable final
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Reglas para compilar archivos .o desde los archivos .c
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Regla para limpiar archivos compilados
clean:
	rm -f $(OBJ) $(TARGET)

# Limpieza más profunda
distclean: clean
	rm -f *~ core

# Ejecutar la aplicación
run: $(TARGET)
	./$(TARGET)

