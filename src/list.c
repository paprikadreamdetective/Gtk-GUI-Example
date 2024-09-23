#include "../include/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void new(LIST *list) {
    list->head = NULL;
    list->size = 0;
    return;
}

void new_node(NODE *node) {
    node->next = NULL;
}

int size(LIST *list) {
    return(list->size);
}

int IsEmpty(LIST list) {
    if(list.head == NULL)
        return(1);
    else
        return(0);
}

void insert(LIST *list, int t_id, char t_nombre[50], char t_categoria[50], float t_precio, int t_cantidad_stock, char t_codigo_barras[50]) {
    // Nodo correspondiente a la creacion de la cabeza de la lista
    NODE *head_list = list->head;
    // Nodo actual que nos servira para encontrar el punto de inserccion
    NODE *current_node = head_list;
    // Nodo anterior que sirve como referencia para hacer insercciones
    NODE *previous_node = head_list;
    // Un nodo auxiliar para poder accesar a los campos y modificarlos
    NODE *aux;

    if(IsEmpty(*list) == 1) {
        head_list = (NODE*)malloc(sizeof(NODE));
        new_node(head_list);
	
	head_list->data.id = t_id;
        strcpy(head_list->data.nombre, t_nombre);
        strcpy(head_list->data.categoria, t_categoria);
	head_list->data.precio = t_precio;
	head_list->data.cantidad_stock = t_cantidad_stock;
	strcpy(head_list->data.codigo_barras, t_codigo_barras);
        
        list->head = head_list;
        list->size++;
        return;
    }
        /*Encontrar el punto de inserccion*/
    // En este punto se recorre la lista tomando como referencia el ID de los elementos
    // para saber donde insertarlo de manera ordenada y para poder generar el siguiente elemento
    // y asi insertarlo en el caso que le corresponda, ya sea en medio o en la cola de la lista.
    while(current_node != NULL && t_id > current_node->data.id) {
        previous_node = current_node;
        current_node = current_node->next;
    }
    aux = (NODE*)malloc(sizeof(NODE));
    new_node(aux); /*Se genera un nuevo NODE*/
    if(previous_node == current_node) {
        //strcpy(aux->data.archivo, t_name);
        //aux->data.id = t_id;
        
	aux->data.id = t_id;
        strcpy(aux->data.nombre, t_nombre);
        strcpy(aux->data.categoria, t_categoria);
	aux->data.precio = t_precio;
	aux->data.cantidad_stock = t_cantidad_stock;
	strcpy(aux->data.codigo_barras, t_codigo_barras);
    
	    
	aux->next = head_list; // Se inserta el nodo aux al inicio de la lista
        head_list = aux;        // Se actualiza la cabeza de la lista
        list->size++;
    } else {
        //strcpy(aux->data.archivo, t_name);
        //aux->data.ID = t_id;
        
	aux->data.id = t_id;
        strcpy(aux->data.nombre, t_nombre);
        strcpy(aux->data.categoria, t_categoria);
	aux->data.precio = t_precio;
	aux->data.cantidad_stock = t_cantidad_stock;
	strcpy(aux->data.codigo_barras, t_codigo_barras);
    
	    
	aux->next = current_node;
        previous_node->next = aux; // Se inserta al nodo aux despues de previous_node
        list->size++;              // el cual sirvio como referencia para saber donde hacer la
    }                               // la inserccion.
    list->head = head_list; // Por ultimo, se actualiza la cabeza de la lista que se recibe como
}                           // como parametro en la funcion

void erase(LIST *list, int index) {
    NODE *head_list = list->head;
    NODE *current_node = head_list;
    NODE *previous_node = head_list;
    if(IsEmpty(*list) == 1) {
        printf(">> Lista vacia. \n");
        printf(">> No hay ningun archivo procesandose. \n");
        return;
    }
    /*Entrar en la lista y encontrar el elemento a borrar*/
    while(current_node != NULL && index != current_node->data.id) {
        previous_node = current_node;
        current_node = current_node->next;
    }
    /*Si el dato no se encuentra: */
    if(current_node == NULL) {
        printf("\n");
        printf(">> Proceso no encontrado. \n");
        return;
    }
    /*Si el dato se encuentra: */
    if(previous_node == current_node) {
        head_list = head_list->next;/*Borrar un elemento de la cabeza*/
        printf("\n");
        printf(">> Proceso terminado: \n");
    } else {
        previous_node->next = current_node->next; /*Borrar elemento que no sea la cabeza*/
        printf("\n");
        printf(">> Proceso terminado: \n");
    }
    free(current_node);
    list->head = head_list;
    list->size--;
}

void search(LIST *list, int index) {
    NODE *current_node = list->head;
    while(current_node != NULL && index != current_node->data.id)
        current_node = current_node->next;
    if(current_node) {
        printf(">> Producto: \n");
	printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("|   ID   |                      Nombre                    |      Categoria       |        Precio        |         Stock        |   Codigo Barras      |\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("| %-6d | %-46s | %-20s | %-20.2f | %-20d | %-20s | \n", current_node->data.id, current_node->data.nombre, current_node->data.categoria, current_node->data.precio, current_node->data.cantidad_stock, current_node->data.codigo_barras);
        printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    } else {
        printf("\n");
        printf("\t\t >> Producto no encontrado. \n\n");
    }
}


void display(LIST *list) {
    NODE *aux = list->head;
    DATA dat;
    
    if (IsEmpty(*list) == 1) {
        printf("\n\t\t>> Inventario Vacio \n");
    } else {
        printf(">> Inventario \n");
        printf(">> Total de productos: %d\n", size(list));
        printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("|   ID   |     	  		Nombre			  |   	 Categoria 	 |        Precio        |         Stock        |   Codigo Barras      |\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        
        while (aux != NULL) {
            dat = aux->data;
            printf("| %-6d | %-46s | %-20s | %-20.2f | %-20d | %-20s | \n", dat.id, dat.nombre, dat.categoria, dat.precio, dat.cantidad_stock, dat.codigo_barras);
            aux = aux->next;
        }
        
        printf("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
}


