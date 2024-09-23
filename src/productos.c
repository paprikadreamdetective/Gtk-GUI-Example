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




void mostrar_productos(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog, *content_area, *vbox;
    GtkWidget *label, *entry_id, *button_ok;

    // Crear ventana de diálogo
    dialog = gtk_dialog_new_with_buttons("Productos Disponibles",
                                         GTK_WINDOW(window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                         NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

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

    // Mostrar productos
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

    // Añadir el VBox al área de contenido del diálogo
    gtk_container_add(GTK_CONTAINER(content_area), vbox);
    gtk_widget_show_all(dialog);

    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }

    close(shm_fd);
}


void terminar_aplicacion(GtkWidget *widget, gpointer window) {
    gtk_main_quit();
}


