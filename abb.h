#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>


/* ******************************************************************
 *                        TIPOS DE DATOS
 * *****************************************************************/

typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);


/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

/* Crea el abb*/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Guarda un elemento en el abb, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura abb fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/* Borra un elemento del abb y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura abb fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *abb_borrar(abb_t *arbol, const char *clave);

/* Obtiene el valor de un elemento del abb, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura abb fue inicializada
 */
void *abb_obtener(const abb_t *arbol, const char *clave);

/* Determina si clave pertenece o no al abb.
 * Pre: La estructura abb fue inicializada
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);

/* Devuelve la cantidad de elementos del abb.
 * Pre: La estructura abb fue inicializada
 */
size_t abb_cantidad(const abb_t *arbol);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura abb fue inicializada
 * Post: La estructura abb fue destruida
 */
void abb_destruir(abb_t *arbol);


/* ******************************************************************
 *                    PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

/* Pre: Recibe el arbol a iterar, una funcion visitar para aplicar a cada elemento del arbol,
 * y un elemento extra para compartir informacion entre interaciones.
 * Post: Itera el arbol elemento por elemento aplicando la funcion visitar a cada uno de ellos, 
 * utilizando el elemento extra si se paso como parametro a visitar. Si en alguna iteracion
 * la funcion visitar devuelve false, se interrumpe la iteracion. En caso contrario, se itera
 * hasta el ultimo elemento del arbol.
 */
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);


/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea iterador
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza iterador
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Comprueba si terminó la iteración
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye iterador
void abb_iter_in_destruir(abb_iter_t* iter);

#endif  // ABB_H
