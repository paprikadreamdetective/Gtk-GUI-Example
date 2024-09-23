#include <gtk/gtk.h>
#include "../include/productos.h"
#include "../include/memoria_compartida.h"

// Función para cambiar el contenido de la derecha según el botón presionado
void mostrar_vista(GtkWidget *widget, gpointer stack) {
    const gchar *page_name = (const gchar*)g_object_get_data(G_OBJECT(widget), "page_name");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), page_name);
}

void iniciar_aplicacion(GtkWidget *widget, gpointer data) {
    GtkWidget *window;
    GtkWidget *hbox;
    GtkWidget *vbox_left;
    GtkWidget *stack;
    GtkWidget *button_visualizar, *button_leer, *button_comprar, *button_terminar;
    GtkWidget *label_vista1, *label_vista2, *label_vista3;
    GtkWidget *label_resultado1, *label_resultado2, *label_resultado3;
    
    // Crear ventana principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Steren Manager");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crear contenedor horizontal (hbox) para dividir en dos columnas
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // Crear la columna izquierda (vbox_left) para los botones
    vbox_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox_left, FALSE, FALSE, 5);

    // Crear el GtkStack para la derecha (contenido cambiante)
    stack = gtk_stack_new();
    gtk_box_pack_start(GTK_BOX(hbox), stack, TRUE, TRUE, 5);

    // Crear widgets para el contenido del stack
    label_vista1 = gtk_label_new("Cargar Productos:");
    label_resultado1 = gtk_label_new("");
    GtkWidget *vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox1), label_vista1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), label_resultado1, FALSE, FALSE, 0);

    label_vista2 = gtk_label_new("Visualizar Productos:");
    label_resultado2 = gtk_label_new("");
    GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox2), label_vista2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), label_resultado2, FALSE, FALSE, 0);

    label_vista3 = gtk_label_new("Comprar un producto:");
    label_resultado3 = gtk_label_new("");
    GtkWidget *vbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox3), label_vista3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox3), label_resultado3, FALSE, FALSE, 0);

    // Agregar las vistas al GtkStack con nombres únicos
    gtk_stack_add_named(GTK_STACK(stack), vbox1, "vista_cargar");
    gtk_stack_add_named(GTK_STACK(stack), vbox2, "vista_visualizar");
    gtk_stack_add_named(GTK_STACK(stack), vbox3, "vista_compra");

    // Botón Cargar Memoria Compartida
    button_visualizar = gtk_button_new_with_label("Cargar Productos");
    g_object_set_data(G_OBJECT(button_visualizar), "page_name", "vista_cargar");
    g_signal_connect(button_visualizar, "clicked", G_CALLBACK(mostrar_vista), stack);
    g_signal_connect(button_visualizar, "clicked", G_CALLBACK(event_clicked_cargar_memoria_compartida), label_resultado1);
    gtk_box_pack_start(GTK_BOX(vbox_left), button_visualizar, TRUE, TRUE, 0);

    // Botón Visualizar Productos
    button_leer = gtk_button_new_with_label("Visualizar Productos");
    g_object_set_data(G_OBJECT(button_leer), "page_name", "vista_visualizar");
    g_signal_connect(button_leer, "clicked", G_CALLBACK(mostrar_vista), stack);
    g_signal_connect(button_leer, "clicked", G_CALLBACK(leer_memoria_compartida), label_resultado2);
    gtk_box_pack_start(GTK_BOX(vbox_left), button_leer, TRUE, TRUE, 0);

    // Botón Compra de Productos
    button_comprar = gtk_button_new_with_label("Compra de Productos");
    g_object_set_data(G_OBJECT(button_comprar), "page_name", "vista_compra");
    g_signal_connect(button_comprar, "clicked", G_CALLBACK(mostrar_vista), stack);
    g_signal_connect(button_comprar, "clicked", G_CALLBACK(mostrar_productos), vbox3);
    gtk_box_pack_start(GTK_BOX(vbox_left), button_comprar, TRUE, TRUE, 0);

    // Botón Terminar
    button_terminar = gtk_button_new_with_label("Terminar");
    g_signal_connect(button_terminar, "clicked", G_CALLBACK(terminar_aplicacion), NULL);
    gtk_box_pack_start(GTK_BOX(vbox_left), button_terminar, TRUE, TRUE, 0);

    // Mostrar todos los widgets
    gtk_widget_show_all(window);    
    
    /*
    // Crear ventana principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Steren Manager");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crear contenedor horizontal (hbox) para dividir en dos columnas
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // Crear la columna izquierda (vbox_left) para los botones
    vbox_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox_left, FALSE, FALSE, 5);

    // Crear el GtkStack para la derecha (contenido cambiante)
    stack = gtk_stack_new();
    gtk_box_pack_start(GTK_BOX(hbox), stack, TRUE, TRUE, 5);

    // Crear widgets para el contenido del stack
    label_vista1 = gtk_label_new("Vista: Cargar Productos");
    label_vista2 = gtk_label_new("Vista: Visualizar Productos");
    label_vista3 = gtk_label_new("Vista: Comprar Producto");
    
    label_resultado1 = gtk_label_new("");
    label_resultado2 = gtk_label_new("");


    // Agregar las vistas al GtkStack con nombres únicos
    gtk_stack_add_named(GTK_STACK(stack), label_vista1, "vista_cargar");
    gtk_stack_add_named(GTK_STACK(stack), label_vista2, "vista_visualizar");
    gtk_stack_add_named(GTK_STACK(stack), label_vista3, "vista_comprar");

    // Botón Cargar Productos
    button_visualizar = gtk_button_new_with_label("Cargar Productos");
    g_signal_connect(button_visualizar, "clicked", G_CALLBACK(event_clicked_cargar_memoria_compartida), stack);
    g_object_set_data(G_OBJECT(button_visualizar), "page_name", "vista_cargar");
    gtk_box_pack_start(GTK_BOX(vbox_left), button_visualizar, TRUE, TRUE, 0);

    // Botón Visualizar Productos
    button_leer = gtk_button_new_with_label("Visualizar Productos");
    g_signal_connect(button_leer, "clicked", G_CALLBACK(leer_memoria_compartida), stack);
    g_object_set_data(G_OBJECT(button_leer), "page_name", "vista_visualizar");
    gtk_box_pack_start(GTK_BOX(vbox_left), button_leer, TRUE, TRUE, 0);

    // Botón Comprar Producto
    button_buscar = gtk_button_new_with_label("Comprar Producto");
    g_signal_connect(button_buscar, "clicked", G_CALLBACK(mostrar_vista), stack);
    g_object_set_data(G_OBJECT(button_buscar), "page_name", "vista_comprar");
    gtk_box_pack_start(GTK_BOX(vbox_left), button_buscar, TRUE, TRUE, 0);

    // Botón Terminar
    button_terminar = gtk_button_new_with_label("Terminar");
    g_signal_connect(button_terminar, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(vbox_left), button_terminar, TRUE, TRUE, 0);

    // Mostrar todos los widgets
    gtk_widget_show_all(window);*/
}





/*
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
}*/
