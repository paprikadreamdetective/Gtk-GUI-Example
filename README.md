
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
## Resultados de implementacion.

## Interfaz principal:
![image](https://github.com/user-attachments/assets/41baea05-8b3d-4b10-a485-7ddec9a6ecbc)

## Menu:
![image](https://github.com/user-attachments/assets/2dbbccaf-75e0-4341-81bc-fc9ed45e9de9)

## Opcion "Cargar Productos desde la Memoria Compartida" :
![image](https://github.com/user-attachments/assets/726c26f8-9486-43ea-8d01-350fc9f92c6b)

## Opcion "Visualizar Productos":
![image](https://github.com/user-attachments/assets/9a2d3b9a-7e80-4637-a8ef-f2cbb5e9fc75)

## Opcion "Compra de Productos":
![image](https://github.com/user-attachments/assets/cfda8975-74b5-4210-8884-f441c19063d3)
![image](https://github.com/user-attachments/assets/ba23cb06-be5e-42ca-8280-753cf1151715)
![image](https://github.com/user-attachments/assets/3a0b2834-ee3c-456a-b39d-1ef7b8eed67c)

## Demostracion del uso de la exclusion mutua para bloquear el acceso de un registro seleccionado cuando otra aplicacion quiere realizar la operacion de "Comprar un Producto" sobre ese mismo registro.
![image](https://github.com/user-attachments/assets/ae9ba342-ac99-4224-828a-abd2a943d993)
![image](https://github.com/user-attachments/assets/25da381e-0b16-4221-a8bd-ae1934b5237e)
![image](https://github.com/user-attachments/assets/9f26c2ff-4cdd-4480-8250-0fac87b253a0)
![image](https://github.com/user-attachments/assets/eb0a15d1-3cf5-4cff-8f36-e791e45b9833)
![image](https://github.com/user-attachments/assets/f432fc7e-e352-43b9-a38e-c78beb5dcdea)



