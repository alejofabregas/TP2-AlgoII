#include "heap.h"
#include <stdlib.h>

#define TAM_INICIAL 20
#define FACTOR_REDIMENSION 2
#define FACTOR_CANT_MIN 4


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct heap {
	void** datos;
	size_t cant;
	size_t tam;
	cmp_func_t cmp;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* PRE: Recibe el heap y un nuevo tamaño de capacidad
 * POST: Devuelve true si se pudo redimensionar sino false
 */
bool heap_redimensionar(heap_t *heap, size_t tam) {
	void **datos_nuevo = realloc(heap->datos, tam * sizeof(void*));
	if (datos_nuevo == NULL) return false;
	heap->datos = datos_nuevo;
	heap->tam = tam;
	return true;
}

/* PRE: Recibe dos punteros genericos
 * POST: Invierte los punteros recibidos
 */
void swap(void** x, void** y) {
	void* aux = *x;
	*x = *y;
	*y = aux;
}

/* PRE: Recibe un arreglo, una posicion y una funcion de comparacion
 * POST: Llama recursivamente desde abajo hasta llegar al incio asegurando que se cumpla prop de max-heap
 */
void upheap(void* elementos[], cmp_func_t cmp, size_t pos_elemento) {
	if (pos_elemento == 0) return; // El elemento es la raiz
	size_t pos_padre = (pos_elemento - 1) / 2;
	if (cmp(elementos[pos_padre], elementos[pos_elemento]) < 0) {
		swap(&elementos[pos_padre], &elementos[pos_elemento]);
		upheap(elementos, cmp, pos_padre);
	}
}

/* PRE: Recibe un arreglo de genericos, una funcion de comparacion, una pos de padre, hijo izq e hijo der
 * POST: Devuelve la posicion del maximo de los tres
 */
size_t calcular_pos_hijo_max(void* elementos[], cmp_func_t cmp, size_t cant_elem, size_t pos_elemento) {
	size_t pos_hijo_izq = 2 * pos_elemento + 1;
	if (pos_hijo_izq >= cant_elem) pos_hijo_izq = 0;
	size_t pos_hijo_der = 2 * pos_elemento + 2;
	if (pos_hijo_der >= cant_elem) pos_hijo_der = 0;
	size_t pos_max = 0;
	if (pos_hijo_izq && pos_hijo_der) {
		if (cmp(elementos[pos_hijo_izq], elementos[pos_hijo_der]) >= 0) pos_max = pos_hijo_izq;
		else pos_max = pos_hijo_der;
	} else {
		if (pos_hijo_izq) pos_max = pos_hijo_izq;
		if (pos_hijo_der) pos_max = pos_hijo_der;
	} 
	return pos_max;
}

/* PRE: Recibe un arreglo, una posicion y una funcion de comparacion
 * POST: Llama recursivamente desde arriba hasta llegar al final asegurando que se cumpla prop de max-heap
 */
void downheap(void* elementos[], cmp_func_t cmp, size_t cant_elem, size_t pos_elemento) {
	if (pos_elemento == cant_elem - 1) return;
	size_t pos_hijo_max = calcular_pos_hijo_max(elementos, cmp, cant_elem, pos_elemento);
	if (!pos_hijo_max) return;
	if (cmp(elementos[pos_elemento], elementos[pos_hijo_max]) < 0) {
		swap(&elementos[pos_elemento], &elementos[pos_hijo_max]);
		downheap(elementos, cmp, cant_elem, pos_hijo_max);
	}
}

/* PRE: Recibe un arreglo, la cantidad de elementos y una funcion de comparacion
 * POST: Se le da forma de heap al arreglo, es decir que cumple la propiedad de heap, aplicando downheap del ultimo al primer elemento
 */
void heapify(void* elementos[], size_t cant, cmp_func_t cmp) {
	for (size_t i = cant / 2; i > 0; i--) {
		downheap(elementos, cmp, cant, i - 1);
	}
}


/* ******************************************************************
 *                    PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t* heap_crear(cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	heap->cant = 0;
	heap->tam = TAM_INICIAL;
	heap->cmp = cmp;
	void** elementos = malloc(heap->tam * sizeof(void*));
	if (!elementos) {
		free(heap);
		return NULL; 
	}
	heap->datos = elementos;
	return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp) {
	heap_t* heap = heap_crear(cmp);
	if (!heap) return NULL;
	heap->cant = n;
	if (n >= heap->tam) {
		if (!heap_redimensionar(heap, n * FACTOR_REDIMENSION)) return NULL;
	}
	for (size_t i = 0; i < n; i++) {
		heap->datos[i] = arreglo[i];
	}
	heapify(heap->datos, n, cmp);
	return heap;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)) {
	if (destruir_elemento) {
		for (size_t i = 0; i < heap->cant; i++) {
			destruir_elemento(heap->datos[i]);
		}
	}
	free(heap->datos);
	free(heap);
}

size_t heap_cantidad(const heap_t *heap) {
	return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap) {
	return heap->cant == 0;
}

bool heap_encolar(heap_t *heap, void *elem) {
	if (!heap) return false;
	if (heap->cant == heap->tam) {
		if (!heap_redimensionar(heap, heap->tam * FACTOR_REDIMENSION)) return false;
	}
	heap->datos[heap->cant] = elem;
	upheap(heap->datos, heap->cmp, heap->cant);
	heap->cant++;
	return true;
}

void *heap_ver_max(const heap_t *heap) {
	return heap_esta_vacio(heap) ? NULL : heap->datos[0];
}

void *heap_desencolar(heap_t *heap) {
	if (heap_esta_vacio(heap)) return NULL;
	void* desencolado = heap->datos[0];
	swap(&heap->datos[0], &heap->datos[heap->cant - 1]);
	heap->cant--;
	downheap(heap->datos, heap->cmp, heap->cant, 0);
	if (heap->cant * FACTOR_CANT_MIN <= heap->tam && heap->tam / FACTOR_REDIMENSION >= TAM_INICIAL) {
		heap_redimensionar(heap, heap->tam / FACTOR_REDIMENSION);
	}
	return desencolado;
}


/* ******************************************************************
 *                          HEAPSORT
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp) {
	if (cant == 0) return;
	heapify(elementos, cant, cmp);
	for (size_t i = cant - 1; i > 0; i--) {
		swap(&elementos[0], &elementos[i]);
		downheap(elementos, cmp, i, 0);
	}
}
