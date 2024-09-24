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
Esta funcion retorna una estructura de tipo ```c LIST ``` que contiene los registros ya guardados, para posteriormente invocar esta funcion y cargarlos a memoria compartida.

