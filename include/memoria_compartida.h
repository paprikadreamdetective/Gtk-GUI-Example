#ifndef MEMORIA_COMPARTIDA_H
#define MEMORIA_COMPARTIDA_H

#include "list.h"

#define SHM_NAME "/product_shm"
#define MAX_PRODUCTOS 100

typedef struct {
    int id;
    char nombre[50];
    char categoria[50];
    float precio;
    int cantidad_stock;
    char codigo_barras[50];
    pthread_mutex_t mutex;
} PRODUCTO;

void cargar_memoria_compartida(LIST *list, GtkWidget *label_resultado);
void leer_memoria_compartida(GtkWidget *widget, gpointer label_resultado);
void event_clicked_cargar_memoria_compartida(GtkWidget *widget, gpointer window);





#endif
