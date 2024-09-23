#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/productos.h"
#include "../include/memoria_compartida.h"
#include "../include/list.h"
#include <pthread.h>



void confirmar_compra(GtkWidget *widget, gpointer entry) {
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    int id = atoi(entry_text);

    // Abrir memoria compartida
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error abriendo la memoria compartida");
        exit(1);
    }

    size_t size = MAX_PRODUCTOS * sizeof(PRODUCTO);
    PRODUCTO *productos = (PRODUCTO *)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (productos == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        if (productos[i].id == id) {
            if (pthread_mutex_lock(&productos[i].mutex) != 0) {
                perror("Error al bloquear el mutex");
                return;
            }

            GtkWidget *dialog;
            dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_QUESTION,
                                            GTK_BUTTONS_YES_NO,
                                            "¿Deseas proceder con la compra del libro '%s'?", productos[i].nombre);
            gint response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if (response == GTK_RESPONSE_YES) {
                if (productos[i].cantidad_stock > 0) {
                    productos[i].cantidad_stock--;
                    g_print("Compra realizada con éxito. Stock restante: %d\n", productos[i].cantidad_stock);
                } else {
                    g_print("Stock insuficiente.\n");
                }
            } else {
                g_print("Compra cancelada.\n");
            }

            if (pthread_mutex_unlock(&productos[i].mutex) != 0) {
                perror("Error al desbloquear el mutex");
            }
            break;
        }
    }

    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}


void mostrar_productos(GtkWidget *widget, gpointer vbox_derecha) {
    GtkWidget *grid;
    GtkWidget *label_id, *label_nombre, *label_categoria, *label_precio, *label_stock, *label_codigo, *entry_id, *button_ok;

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

    // Cargar productos desde la memoria compartida
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
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

    // Campo para ingresar ID y botón de confirmación, debajo de la lista de productos
    GtkWidget *label_ingresar_id = gtk_label_new("Ingresa el ID del producto:");
    entry_id = gtk_entry_new();
    button_ok = gtk_button_new_with_label("Confirmar compra");

    // Añadir widgets de entrada y botón debajo de la grid
    gtk_grid_attach(GTK_GRID(grid), label_ingresar_id, 0, MAX_PRODUCTOS + 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_id, 0, MAX_PRODUCTOS + 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), button_ok, 0, MAX_PRODUCTOS + 3, 2, 1);

    // Conectar la señal del botón a la función de confirmar compra
    g_signal_connect(button_ok, "clicked", G_CALLBACK(confirmar_compra), entry_id);

    // Actualizar la ventana
    gtk_widget_show_all(vbox_derecha);

    // Liberar recursos de la memoria compartida
    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}



/*
void mostrar_productos(GtkWidget *widget, gpointer vbox_derecha) {
    GtkWidget *vbox;
    GtkWidget *label, *entry_id, *button_ok;

    // Limpiar el contenido anterior en la parte derecha (vbox_derecha)
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(vbox_derecha));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Crear un VBox nuevo dentro de la parte derecha
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox_derecha), vbox, TRUE, TRUE, 0);

    // Cargar productos desde la memoria compartida
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("Error abriendo la memoria compartida");
        exit(1);
    }

    size_t size = MAX_PRODUCTOS * sizeof(PRODUCTO);
    PRODUCTO *productos = (PRODUCTO *)mmap(0, size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (productos == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    // Mostrar productos disponibles en la parte derecha
    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        label = gtk_label_new(g_strdup_printf("ID: %d, Producto: %s, Stock: %d",
                                              productos[i].id,
                                              productos[i].nombre,
                                              productos[i].cantidad_stock));
        gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    }

    // Campo para ingresar ID y botón de confirmación
    GtkWidget *label_ingresar_id = gtk_label_new("Ingresa el ID del producto:");
    entry_id = gtk_entry_new();
    button_ok = gtk_button_new_with_label("Confirmar compra");

    gtk_box_pack_start(GTK_BOX(vbox), label_ingresar_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_ok, FALSE, FALSE, 0);

    // Conectar la señal del botón a la función de confirmar compra
    g_signal_connect(button_ok, "clicked", G_CALLBACK(confirmar_compra), entry_id);

    // Actualizar la ventana
    gtk_widget_show_all(vbox_derecha);

    // Liberar recursos de la memoria compartida
    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}*/

void terminar_aplicacion(GtkWidget *widget, gpointer window) {
    gtk_main_quit();
}


