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


