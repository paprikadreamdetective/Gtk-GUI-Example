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
/*
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
    int producto_encontrado = 0;
    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        if (productos[i].id == id) {
	    producto_encontrado = 1;	
            if (pthread_mutex_lock(&productos[i].mutex) != 0) {
                perror("Error al bloquear el mutex");
                return;
            }
	    if (productos[i].cantidad_stock == 0) {
	    	GtkWidget *dialog;
                dialog = gtk_message_dialog_new(NULL,
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK,
                                                "El producto '%s' está agotado.", productos[i].nombre);
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);

                pthread_mutex_unlock(&productos[i].mutex);
                break;
	    }
	  



            GtkWidget *dialog, *content_area, *grid, *label_pregunta, *button_box;
            dialog = gtk_dialog_new_with_buttons("Confirmar Compra",
			    		    NULL,
                                            GTK_DIALOG_MODAL,
                                            NULL);
            content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	    // Crear un GtkGrid para mostrar el producto encontrado
            grid = gtk_grid_new();
            gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
            gtk_grid_set_column_spacing(GTK_GRID(grid), 20);


	    gtk_widget_set_halign(GTK_WIDGET(grid), GTK_ALIGN_CENTER);
	    gtk_widget_set_valign(GTK_WIDGET(grid), GTK_ALIGN_CENTER);
	    gtk_widget_set_hexpand(GTK_WIDGET(grid), TRUE);
	    gtk_widget_set_vexpand(GTK_WIDGET(grid), TRUE);


            GtkWidget *label_id = gtk_label_new("ID");
            GtkWidget *label_nombre = gtk_label_new("Nombre");
            GtkWidget *label_categoria = gtk_label_new("Categoria");
            GtkWidget *label_precio = gtk_label_new("Precio");
            GtkWidget *label_stock = gtk_label_new("Stock");
            GtkWidget *label_codigo = gtk_label_new("Codigo de barras");
		
	    
            GtkWidget *value_id = gtk_label_new(g_strdup_printf("%d", productos[i].id));
            GtkWidget *value_nombre = gtk_label_new(productos[i].nombre);
	    GtkWidget *value_categoria = gtk_label_new(productos[i].categoria);
            GtkWidget *value_precio = gtk_label_new(g_strdup_printf("%.2f", productos[i].precio));
            GtkWidget *value_stock = gtk_label_new(g_strdup_printf("%d", productos[i].cantidad_stock));
	    GtkWidget *value_codigo = gtk_label_new(productos[i].codigo_barras);

            // Colocar etiquetas en el grid
            gtk_grid_attach(GTK_GRID(grid), label_id, 0, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_nombre, 1, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_categoria, 2, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_precio, 3, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_stock, 4, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_codigo, 5, 0, 1, 1);

            gtk_grid_attach(GTK_GRID(grid), value_id, 0, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_nombre, 1, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_categoria, 2, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_precio, 3, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_stock, 4, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_codigo, 5, 1, 1, 1);

	    label_pregunta = gtk_label_new("¿Desea hacer la compra?");
	    gtk_widget_set_halign(label_pregunta, GTK_ALIGN_CENTER);
	    gtk_widget_set_valign(label_pregunta, GTK_ALIGN_CENTER);
	    PangoFontDescription *font_desc = pango_font_description_new();
	    pango_font_description_set_size(font_desc, 18 * PANGO_SCALE); // Tamaño grande
	    //gtk_widget_override_font(label_pregunta, font_desc);
	    pango_font_description_free(font_desc);
	    gtk_box_pack_start(GTK_BOX(content_area), label_pregunta, FALSE, FALSE, 30);
	    

	    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_box_pack_start(GTK_BOX(button_box), label_pregunta, TRUE, TRUE, 10);
	    
	   
            // Agregar botones a la caja horizontal
            GtkWidget *button_yes = gtk_button_new_with_label("Sí");
            GtkWidget *button_no = gtk_button_new_with_label("No");
            gtk_box_pack_start(GTK_BOX(button_box), button_yes, FALSE, FALSE, 10);
            gtk_box_pack_start(GTK_BOX(button_box), button_no, FALSE, FALSE, 10);	    

	    
	      // Empacar el GtkBox en el área de contenido
            gtk_box_pack_start(GTK_BOX(content_area), button_box, FALSE, FALSE, 10);

            gtk_container_add(GTK_CONTAINER(content_area), grid);
            gtk_widget_show_all(dialog);

            // Esperar respuesta del usuario
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

    // Mostrar "Producto no encontrado" si no existe el ID
    if (!producto_encontrado) {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "Producto no encontrado.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    if (munmap(productos, size) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(1);
    }
    close(shm_fd);
}*/



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

    int producto_encontrado = 0;
    for (int i = 0; i < MAX_PRODUCTOS && productos[i].id != 0; i++) {
        if (productos[i].id == id) {
            producto_encontrado = 1;

            // Bloqueo del mutex para el producto
            //if (pthread_mutex_lock(&productos[i].mutex) != 0) {
            if (pthread_mutex_trylock(&productos[i].mutex) != 0) {
                perror("Error al bloquear el mutex");
                return;
            }

            // Si el producto está agotado
            if (productos[i].cantidad_stock == 0) {
                GtkWidget *dialog;
                dialog = gtk_message_dialog_new(NULL,
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK,
                                                "El producto '%s' está agotado.", productos[i].nombre);
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);

                pthread_mutex_unlock(&productos[i].mutex);
                break;
            }

            // Crear un nuevo diálogo
            GtkWidget *dialog, *content_area, *grid, *label_pregunta, *button_box;
            dialog = gtk_dialog_new_with_buttons("Confirmar Compra",
                                                 NULL,
                                                 GTK_DIALOG_MODAL,
                                                 "_Sí", GTK_RESPONSE_YES,
                                                 "_No", GTK_RESPONSE_NO,
                                                 NULL);
            content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

            // Crear un GtkGrid para mostrar el producto encontrado
            grid = gtk_grid_new();
            gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
            gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

            // Etiquetas de los detalles del producto
            GtkWidget *label_id = gtk_label_new("ID");
            GtkWidget *label_nombre = gtk_label_new("Nombre");
            GtkWidget *label_categoria = gtk_label_new("Categoria");
            GtkWidget *label_precio = gtk_label_new("Precio");
            GtkWidget *label_stock = gtk_label_new("Stock");
            GtkWidget *label_codigo = gtk_label_new("Codigo de barras");

            // Valores del producto
            GtkWidget *value_id = gtk_label_new(g_strdup_printf("%d", productos[i].id));
            GtkWidget *value_nombre = gtk_label_new(productos[i].nombre);
            GtkWidget *value_categoria = gtk_label_new(productos[i].categoria);
            GtkWidget *value_precio = gtk_label_new(g_strdup_printf("%.2f", productos[i].precio));
            GtkWidget *value_stock = gtk_label_new(g_strdup_printf("%d", productos[i].cantidad_stock));
            GtkWidget *value_codigo = gtk_label_new(productos[i].codigo_barras);

            // Colocar etiquetas en el grid
            gtk_grid_attach(GTK_GRID(grid), label_id, 0, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_nombre, 1, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_categoria, 2, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_precio, 3, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_stock, 4, 0, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), label_codigo, 5, 0, 1, 1);

            gtk_grid_attach(GTK_GRID(grid), value_id, 0, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_nombre, 1, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_categoria, 2, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_precio, 3, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_stock, 4, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value_codigo, 5, 1, 1, 1);

            // Agregar el grid al área de contenido del diálogo
            gtk_box_pack_start(GTK_BOX(content_area), grid, TRUE, TRUE, 10);

            // Crear una caja horizontal para la pregunta y los botones
            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            label_pregunta = gtk_label_new("¿Desea hacer la compra?");
            gtk_box_pack_start(GTK_BOX(hbox), label_pregunta, TRUE, TRUE, 10);

            // Agregar botones a la caja horizontal
            /*GtkWidget *button_yes = gtk_button_new_with_label("Sí");
            GtkWidget *button_no = gtk_button_new_with_label("No");
            gtk_box_pack_start(GTK_BOX(hbox), button_yes, FALSE, FALSE, 10);
            gtk_box_pack_start(GTK_BOX(hbox), button_no, FALSE, FALSE, 10);*/

            // Empacar la caja horizontal en el área de contenido
            gtk_box_pack_start(GTK_BOX(content_area), hbox, FALSE, FALSE, 10);

            // Mostrar el diálogo
            gtk_widget_show_all(dialog);

            // Esperar respuesta del usuario
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

            // Desbloquear el mutex
            if (pthread_mutex_unlock(&productos[i].mutex) != 0) {
                perror("Error al desbloquear el mutex");
            }
            break;
        }
    }

    // Mostrar "Producto no encontrado" si no existe el ID
    if (!producto_encontrado) {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "Producto no encontrado.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }

    // Desmapear la memoria compartida y cerrar el archivo
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

void terminar_aplicacion(GtkWidget *widget, gpointer window) {
    gtk_main_quit();
}


