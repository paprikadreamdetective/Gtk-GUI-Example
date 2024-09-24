# Uso de exclusion mutua y memoria compartida.

El programa consiste en el uso de exlusion mutua y memoria compartida para gestionar la concurrencia.

## 1. Lectura del archivo fuente para cargarlo a la memoria compartida.
En esta parte se emplearon 5 registros para poder guardarlos en la memoria compartida, este proceso se hizo se describe a continuacion.
## 1.1. Obtener el archivo fuente:
Para esto se hizo uso del siguiente codigo:
```c
LIST *lista_productos() {	
	LIST *list = (LIST*)malloc(sizeof(LIST));
	new(list);
	int numRegistros;
	int id;
	char nombre[50];
	char categoria[50];
	float precio;
	int cantidad_stock;
	char codigo_barras[50];
	FILE *fichero;
	fichero = fopen("data.txt", "r");
	fscanf(fichero, "%d", &numRegistros);
	if (fichero == NULL) {
		printf("File error\n");
		return NULL;
	}
	printf("Total registros: %d\n", numRegistros);
	for (int i = 0; i < numRegistros; ++i) {
		fscanf(fichero, "%d\n", &id);
		fscanf(fichero, "%[^\n]\n", nombre);
		fscanf(fichero, "%[^\n]\n", categoria);
		fscanf(fichero, "%f\n", &precio);
		fscanf(fichero, "%d\n", &cantidad_stock);
		fscanf(fichero, "%[^\n]\n", codigo_barras);
		insert(list, id, nombre, categoria, precio, cantidad_stock, codigo_barras);
	}
	display(list);
	fclose(fichero);
	//free(list);
	return list;
}
```
Esta funcion retorna una estructura de tipo ```c LIST ``` que contiene los registros ya

## 1.2. Cargar lo registros a la memoria compartida.
Funcion para cargar registros a la memoria compartida:
```c
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
```

Esta función cargar_memoria_compartida tiene como objetivo cargar los registros de una lista a un espacio de memoria compartida, para que puedan ser accedidos por otros procesos de forma concurrente. La función también inicializa un mutex para cada producto, garantizando que solo un proceso a la vez pueda modificar los datos de un producto específico.

Estructura PRODUCTO:
```c
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

```
## 2. Exclusion mutua hacia cada uno de los registros de la memoria compartida.
La función confirmar_compra tiene como objetivo gestionar el proceso de compra de un producto, interactuando con la memoria compartida para acceder a los productos disponibles y actualizando la cantidad en stock de manera segura mediante el uso de mutex. A continuación se describe el flujo principal:
```c

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
            GtkWidget *dialog, *content_area, *grid, *label_pregunta;
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


```


Descripcion
- Entrada del Usuario:

Obtiene el ID del producto a partir de una entrada de texto en la interfaz gráfica GTK.
- Acceso a la Memoria Compartida:

Abre la memoria compartida usando shm_open y la mapea en el espacio de direcciones del proceso con mmap.
Si el mapeo falla, muestra un mensaje de error y termina la ejecución.
- Búsqueda del Producto:

Recorre los productos almacenados en la memoria compartida buscando aquel cuyo ID coincida con el ingresado.
Si se encuentra el producto, intenta bloquear su mutex con pthread_mutex_trylock para asegurarse de que ningún otro proceso esté accediendo simultáneamente.
- Manejo de Producto Agotado:

Si el producto tiene cantidad_stock igual a 0, se notifica al usuario mediante un diálogo emergente de GTK que el producto está agotado, y el mutex se libera.
- Diálogo de Confirmación:

Muestra un diálogo con los detalles del producto (ID, nombre, categoría, precio, stock, código de barras) y pregunta si el usuario desea realizar la compra.
Si el usuario acepta, se decrementa el cantidad_stock del producto en 1.
Si el usuario cancela la compra, no se realiza ningún cambio.
- Desbloqueo del Mutex:

Después de realizar o cancelar la compra, el mutex del producto se desbloquea con pthread_mutex_unlock.
- Manejo de Producto No Encontrado:

Si no se encuentra un producto con el ID ingresado, se muestra un mensaje emergente informando que el producto no existe.
Liberación de Recursos:

Al finalizar, la memoria compartida se desmapea y el descriptor de archivo asociado se cierra.
## 3. Resultados de implementacion.

## 3.1. Interfaz principal:
![image](https://github.com/user-attachments/assets/41baea05-8b3d-4b10-a485-7ddec9a6ecbc)

