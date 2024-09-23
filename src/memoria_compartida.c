#include <fcntl.h>
#include <gtk/gtk.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/memoria_compartida.h"
#include "../include/list.h"

void cargar_memoria_compartida(LIST *list, GtkWidget *label_resultado) {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
	gtk_label_set_text(GTK_LABEL(label_resultado), "Error abriendo la memoria compartida");
        perror("Error abriendo la memoria compartida");
        exit(1);
    }

    size_t size = MAX_PRODUCTOS * sizeof(PRODUCTO);
    if (ftruncate(shm_fd, size) == -1) {
        
       gtk_label_set_text(GTK_LABEL(label_resultado), "Error truncando la memoria compartida.");
        close(shm_fd);
	perror("Error al truncar la memoria compartida");
        exit(1);
    }

    PRODUCTO *productos = (PRODUCTO *)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (productos == MAP_FAILED) {

	gtk_label_set_text(GTK_LABEL(label_resultado), "Error mapeando la memoria compartida.");

        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    NODE *current_product = list->head;
    int i = 0;
    while (current_product != NULL && i < MAX_PRODUCTOS) {
        productos[i].id = current_product->data.id;
        strncpy(productos[i].nombre, current_product->data.nombre, sizeof(productos[i].nombre));
        strncpy(productos[i].categoria, current_product->data.categoria, sizeof(productos[i].categoria));
        productos[i].precio = current_product->data.precio;
        productos[i].cantidad_stock = current_product->data.cantidad_stock;
        strncpy(productos[i].codigo_barras, current_product->data.codigo_barras, sizeof(productos[i].codigo_barras));

        if (pthread_mutex_init(&productos[i].mutex, NULL) != 0) {
                perror("Error inicializando mutex");
                exit(1);
        }

        current_product = current_product->next;
        i++;
    }
	
    gtk_label_set_text(GTK_LABEL(label_resultado), "Memoria compartida cargada correctamente.");

    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}

void leer_memoria_compartida(GtkWidget *widget, gpointer label_resultado) {
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {

        gtk_label_set_text(GTK_LABEL(label_resultado), "Error abriendo la memoria compartida.");
	
        perror("Error abriendo la memoria compartida");
        exit(1);
    }

    size_t size = MAX_PRODUCTOS * sizeof(PRODUCTO);
    PRODUCTO *productos = (PRODUCTO *)mmap(0, size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (productos == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    /*GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Productos en memoria compartida:");*/

    char buffer[2048] = "|  ID   |         Nombre          |    Categoría    |  Precio  | Stock |    Código de Barras    |\n";
    strcat(buffer, "-----------------------------------------------------------------------------------------------\n");

    char temp[256];
    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        snprintf(temp, sizeof(temp), "| %-6d | %-20.20s | %-14.14s | %8.2f | %5d | %-20.20s |\n",
                 productos[i].id, productos[i].nombre, productos[i].categoria,
                 productos[i].precio, productos[i].cantidad_stock, productos[i].codigo_barras);
        strcat(buffer, temp);
    }

	gtk_label_set_text(GTK_LABEL(label_resultado), buffer);
    //gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", buffer);
    //gtk_dialog_run(GTK_DIALOG(dialog));
    //gtk_widget_destroy(dialog);

    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}


void event_clicked_cargar_memoria_compartida(GtkWidget *widget, gpointer label_resultado) {
    LIST *productos = lista_productos();
    cargar_memoria_compartida(productos, GTK_WIDGET(label_resultado));
}


