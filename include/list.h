
/**
 *  @file list.h
 */


#ifndef LIST_H
#define LIST_H




/** @struct DATA
*   @brief  Esta estructura contiene los campos correspondientes de los producto.
*   @var DATA::char [50]
*   @var DATA::int AGE
*   @var DATA::int ID
*   @var DATA::char CAREER[50]
*   @var DATA::char DIVISION[10]
*/
typedef struct{
    int id;
    char nombre[50];
    char categoria[50];
    float precio;
    int cantidad_stock;
    char codigo_barras[50];
}DATA;

/** @struct NODE
*   @brief  Esta estructura contiene los campos acceder a la estructura UAMITO.
*   @var NODE::UAMITO data
*   @var NODE::struct NODE *next
*/
typedef struct node{
    DATA data;
    struct node *next;
}NODE;

/** @struct LIST
*   @brief  Esta estructura define a la lista.
*   @var LIST::NODE *head
*   @var LIST::int size
*/
typedef struct{
    NODE *head;
    int size;
}LIST;


/** @fn void new(LIST *list)
 *  @brief Crea e in icializa una lista.
 */
void new(LIST *list);

/** @fn void new_node(NODE *node)
 *  @brief Inicializa un nodo de la estructura.
 */
void new_node(NODE *node);

/** @fn int size(LIST *list)
 *  @brief Se accede al tamaño de la lista.
 *  @param *list Recibe la lista
 *  @return Devuelve el tamaño de la lista.
 */
int size(LIST *list);

/** @fn IsEmpty(LIST list)
 *  @brief Verifica si la lista esta vacia o no.
 *  @param list
 *  @return 1 si la lista contiene elementos, 0 si la lista no contiene elementos.
 */
int IsEmpty(LIST list);

/** @fn void insert(LIST *list, char t_name, int t_age, int t_id, char t_career, char t_division)
 *  @brief Se anade un elemento a la lista.
 *  @param *list    Recibe la lista.
 *  @param t_name   Recibe el nombre como cadena.
 *  @param t_age    Recibe la edad como un entero.
 *  @param t_id     Recibe el id como un entero.
 *  @param t_career Recibe la carrera como una cadena.
 *  @param t_division   Recibe la division como una cadena.
 */
void insert(LIST *list, int t_id, char t_nombre[50], char t_categoria[50], float t_precio, int t_cantidad_stock, char t_codigo_barras[50]);

/** @fn void erase(LIST *list, int index)
 *  @brief Se elimina un elemento de la lista.
 *  @param *list    Recibe la lista.
 *  @param index    Recibe como entero el indice a eliminar de la lista.
 */
void erase(LIST *list, int index);

/** @fn void search(LIST *list, int index)
 *  @brief Se busca un elemento de la lista.
 *  @param *list    Recibe la lista.
 *  @param index    Recibe como entero el indice a buscar de la lista.
 */
void search(LIST *list, int index);

/** @fn void display(LIST *list)
 *  @brief Recorre la lista y la muestra en pantalla.
 *  @param *list    Recibe la lista.
 */
void display(LIST *ist);

LIST *lista_productos();



#endif // LIST_H
