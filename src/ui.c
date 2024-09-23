#include <gtk/gtk.h>
#include "../include/productos.h"
#include "../include/memoria_compartida.h"


void iniciar_aplicacion(GtkWidget *widget, gpointer data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_visualizar, *button_leer, *button_buscar, *button_terminar;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Steren Manager");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);

    vbox = gtk_vbox_new(TRUE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Botón Cargar Memoria Compartida
    button_visualizar = gtk_button_new_with_label("Cargar Productos");
    g_signal_connect(button_visualizar, "clicked", G_CALLBACK(event_clicked_cargar_memoria_compartida), window);
    gtk_box_pack_start(GTK_BOX(vbox), button_visualizar, TRUE, TRUE, 0);

    // Botón Leer Archivo
    button_leer = gtk_button_new_with_label("Visualizar Productos");
    g_signal_connect(button_leer, "clicked", G_CALLBACK(leer_memoria_compartida), window);
    gtk_box_pack_start(GTK_BOX(vbox), button_leer, TRUE, TRUE, 0);

    // Botón Buscar Producto
    button_buscar = gtk_button_new_with_label("Comprar Producto");
    g_signal_connect(button_buscar, "clicked", G_CALLBACK(mostrar_productos), window);
    gtk_box_pack_start(GTK_BOX(vbox), button_buscar, TRUE, TRUE, 0);

    // Botón Terminar
    button_terminar = gtk_button_new_with_label("Terminar");
    g_signal_connect(button_terminar, "clicked", G_CALLBACK(terminar_aplicacion), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_terminar, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
}
