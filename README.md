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
Esta función cargar_memoria_compartida tiene como objetivo cargar los registros de una lista (LIST) a un espacio de memoria compartida, para que puedan ser accedidos por otros procesos de forma concurrente. La función también inicializa un mutex para cada producto, garantizando que solo un proceso a la vez pueda modificar los datos de un producto específico.

Descripción General de la Función
Parámetros
LIST *list: Es la lista que contiene los productos a cargar en la memoria compartida. Cada nodo de la lista (NODE) contiene la información de un producto.
GtkWidget *label_resultado: Un widget de GTK usado para mostrar mensajes de éxito o error en la interfaz gráfica.
Estructura del Código
Apertura de la memoria compartida (shm_open)

shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);: Abre un espacio de memoria compartida con el nombre definido por SHM_NAME. Si no existe, lo crea con permisos de lectura y escritura.
Si shm_open falla, se muestra un mensaje de error y se aborta el programa.
Esta memoria compartida es accesible por otros procesos que conozcan su nombre.
Ajuste del tamaño de la memoria (ftruncate)

ftruncate(shm_fd, size);: Cambia el tamaño del objeto de memoria compartida al tamaño requerido por MAX_PRODUCTOS * sizeof(PRODUCTO). Esto asegura que haya suficiente espacio para almacenar todos los productos.
Si ftruncate falla, se notifica al usuario y se cierra el descriptor de archivo.
Mapeo de la memoria compartida al espacio de direcciones del proceso (mmap)

mmap: Asocia la memoria compartida con el espacio de direcciones del proceso para poder leer y escribir en ella. Se especifica que se necesita tanto permiso de lectura como de escritura (con PROT_READ | PROT_WRITE) y que la memoria es compartida entre procesos (MAP_SHARED).
Si mmap falla, se informa del error y se detiene la ejecución del programa.
Copia de los datos de la lista a la memoria compartida

El código recorre la lista (LIST) y copia cada registro de producto en la memoria compartida. Los campos como id, nombre, categoría, precio, cantidad_stock, y codigo_barras se copian de cada nodo de la lista a la memoria.
El índice i se asegura de no sobrescribir más de MAX_PRODUCTOS productos en la memoria.
Inicialización de Mutex para cada producto

Para evitar problemas de concurrencia (que varios procesos intenten modificar los datos al mismo tiempo), cada producto tiene un mutex inicializado con pthread_mutex_init. Esto permite que, durante la ejecución de una operación sobre un producto, el acceso esté bloqueado para otros procesos hasta que el mutex se libere.

