#ifndef PRODUCTOS_H
#define PRODUCTOS_H

#include <gtk/gtk.h>

void mostrar_productos(GtkWidget *widget, gpointer vbox_derecha);
void confirmar_compra(GtkWidget *widget, gpointer entry);
void terminar_aplicacion(GtkWidget *widget, gpointer window);

#endif

