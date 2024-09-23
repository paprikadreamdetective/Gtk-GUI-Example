#include <stdio.h> /* funciones de I/O standard */
#include <unistd.h> /* funciones estándar de unix como alarm() */
#include <signal.h> /* nombres de macros de señales, y el prototipo de signal() */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/list.h"

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
