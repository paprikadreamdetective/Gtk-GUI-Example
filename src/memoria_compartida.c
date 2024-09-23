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

void leer_memoria_compartida(GtkWidget *widget, gpointer vbox_derecha) {
    GtkWidget *grid;
    GtkWidget *label_id, *label_nombre, *label_categoria, *label_precio, *label_stock, *label_codigo;
    // Limpiar el contenido anterior en la parte derecha (vbox_derecha)
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(vbox_derecha));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    // Crear un GtkGrid nuevo dentro de la parte derecha
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 50);
    
    gtk_widget_set_halign(GTK_WIDGET(grid), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(grid), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(grid), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(grid), TRUE);

    gtk_box_pack_start(GTK_BOX(vbox_derecha), grid, TRUE, TRUE, 0);

    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {

        //gtk_label_set_text(GTK_LABEL(l), "Error abriendo la memoria compartida.");
	
        perror("Error abriendo la memoria compartida");
        exit(1);
    }

    size_t size = MAX_PRODUCTOS * sizeof(PRODUCTO);
    PRODUCTO *productos = (PRODUCTO *)mmap(0, size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (productos == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    // Crear las cabeceras de la tabla
    label_id = gtk_label_new("ID");
    label_nombre = gtk_label_new("Producto");
    label_categoria = gtk_label_new("Categoria");
    label_precio = gtk_label_new("Precio");
    label_stock = gtk_label_new("Stock");
    label_codigo = gtk_label_new("Codigo de barras");
    
    gtk_grid_attach(GTK_GRID(grid), label_id, 0, 0, 1, 1);      // Columna 0, Fila 0
    gtk_grid_attach(GTK_GRID(grid), label_nombre, 1, 0, 1, 1);  // Columna 1, Fila 0
    gtk_grid_attach(GTK_GRID(grid), label_categoria, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_precio, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_stock, 4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_codigo, 5, 0, 1, 1);

    // Mostrar productos disponibles en la cuadrícula
    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        // Crear etiquetas para cada columna
        label_id = gtk_label_new(g_strdup_printf("%d", productos[i].id));
        label_nombre = gtk_label_new(productos[i].nombre);
        label_categoria = gtk_label_new(productos[i].categoria);
        label_precio = gtk_label_new(g_strdup_printf("%f", productos[i].precio));
        label_stock = gtk_label_new(g_strdup_printf("%d", productos[i].cantidad_stock));
        label_codigo = gtk_label_new(productos[i].codigo_barras);
        // Añadir las etiquetas a la grid (i+1 porque la fila 0 está ocupada por las cabeceras)
        gtk_grid_attach(GTK_GRID(grid), label_id, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), label_nombre, 1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), label_categoria, 2, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), label_precio, 3, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), label_stock, 4, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), label_codigo, 5, i + 1, 1, 1);
    }
    gtk_widget_show_all(vbox_derecha);
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


