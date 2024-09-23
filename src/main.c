#include <gtk/gtk.h>
#include "ui.h"

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *vbox;
    GtkWidget *label;
    GtkStyleContext *context;
    
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Inicio - Steren Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    label = gtk_label_new("BIenvenid@ a Steren Manager !");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);

    button = gtk_button_new_with_label("Click Para Iniciar Aplicacion");
    gtk_widget_set_size_request(button, 120, 40);

    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);     
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);  
    
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(iniciar_aplicacion), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

