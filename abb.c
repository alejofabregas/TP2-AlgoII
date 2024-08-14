#define _POSIX_C_SOURCE 200809L

#include "abb.h"
#include "pila.h"
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo {
	struct nodo* izq;
	struct nodo* der;
	char* clave;
	void* dato;
} nodo_t;

struct abb {
	nodo_t* raiz;
	size_t cantidad;
	abb_destruir_dato_t func_dest;
	abb_comparar_clave_t func_cmp;
};

struct abb_iter {
	pila_t* pila;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* PRE: Recibe la clave y un dato.
 * POST: Devuelve un nodo que contiene una copia de la clave y el dato.
 */
nodo_t* nodo_crear(const char* clave, void* dato) {
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->clave = strdup(clave);
	if (!nodo->clave) {
		free(nodo);
		return NULL;
	}
	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;
	return nodo;
}

/* PRE: Recibe un nodo previamente creado.
 * POST: Libera la clave y el nodo.
 */
void nodo_destruir(nodo_t* nodo) {
	free(nodo->clave);
	free(nodo);
}

/* PRE: Recibe un nodo, la funcion de comparacion del arbol y la clave a buscar.
 * POST: Busca el elemento en el arbol. Si esta, devuelve el nodo y su padre, o
 * quien deberia serlo si lo queremos guardar. Si no esta, devuelve NULL.
 */
nodo_t* abb_buscar(nodo_t* nodo, nodo_t** padre, abb_comparar_clave_t cmp, const char* clave, bool guardar_padre) {
	if (!nodo) return NULL;
	int comp = cmp(nodo->clave, clave);
	if (comp == 0) return nodo;
	if (guardar_padre) *padre = nodo;
	if (comp > 0) return abb_buscar(nodo->izq, padre, cmp, clave, guardar_padre);
	return abb_buscar(nodo->der, padre, cmp, clave, guardar_padre);
}

/* PRE: Recibe un nodo que es hijo derecho.
 * POST: Busca el minimo de los mayores siguientes.
 */  
nodo_t* abb_buscar_reemplazo(nodo_t* nodo) { 
	while (nodo->der) {
		nodo = nodo->der;
	}
	return nodo;
}

/* PRE: Recibe un arbol, una clave, un nodo y su padre.
 * POST: Borra elemento de 0 o 1 hijos, y devuelve su dato.
 */
void* abb_borrar_0_1_hijos(abb_t* arbol, const char* clave, nodo_t* borrado, nodo_t* padre) {
	if (!padre) {
		if (borrado->der) arbol->raiz = borrado->der;
		else arbol->raiz = borrado->izq;
	} else {
		if (padre->izq == borrado) {
			if (borrado->izq) padre->izq = borrado->izq;
			else padre->izq = borrado->der;
		} else {
			if (borrado->der) padre->der = borrado->der;
			else padre->der = borrado->izq;
		}
	}
	void* dato = borrado->dato;
	nodo_destruir(borrado);
	arbol->cantidad--;
	return dato;
}

/* PRE: Recibe un arbol, una clave y un nodo.
 * POST: Borra el elemento de 2 hijos, y devuelve su dato.
 */
void* abb_borrar_2_hijos(abb_t* arbol, const char* clave, nodo_t* borrado) {
	void* dato = borrado->dato;
	nodo_t* reemplazo = abb_buscar_reemplazo(borrado->izq);
	void* dato_reemplazo = reemplazo->dato;
	char* clave_reemplazo = strdup(reemplazo->clave);
	abb_borrar(arbol,reemplazo->clave);
	free(borrado->clave);
	borrado->dato = dato_reemplazo;
	borrado->clave = clave_reemplazo;
	return dato;
}

/* PRE: Recibe un nodo y la funcion para destruir los datos del arbol (si la tiene).
 * POST: Se llama recursivamente hasta destruir todos los nodos.
 */
void destruir_nodos(nodo_t* nodo, abb_destruir_dato_t func_dest) {
	if (!nodo) return;
	destruir_nodos(nodo->izq, func_dest);
	destruir_nodos(nodo->der, func_dest);
	if (func_dest) func_dest(nodo->dato);
	nodo_destruir(nodo);
}

/* PRE: Recibe un iterador y un nodo actual.
 * POST: Se llama recursivamente apilando todos los hijos izquierdos hasta que no haya.
 */
void apilar(abb_iter_t* iter, nodo_t* nodo) {
	if (!nodo) return;
	pila_apilar(iter->pila,nodo);
	apilar(iter,nodo->izq);
}


/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
	abb_t* arbol = malloc(sizeof(abb_t));
	if (!arbol) return NULL;
	arbol->raiz = NULL;
	arbol->cantidad = 0;
	arbol->func_dest = destruir_dato;
	arbol->func_cmp = cmp;
	return arbol;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
	nodo_t* padre = NULL;
	nodo_t* nodo = abb_buscar(arbol->raiz, &padre, arbol->func_cmp, clave, true);
	if (!nodo) {
		nodo_t* nodo_nuevo = nodo_crear(clave, dato);
		if (!nodo_nuevo) return false;
		if (!padre) arbol->raiz = nodo_nuevo;
		else {
			if (arbol->func_cmp(padre->clave, clave) > 0) padre->izq = nodo_nuevo;
			else padre->der = nodo_nuevo;
		}
		arbol->cantidad++;
	} else {
		if (arbol->func_dest) arbol->func_dest(nodo->dato);
		nodo->dato = dato;
	}
	return true;
}

void *abb_borrar(abb_t *arbol, const char *clave) {
	nodo_t* padre = NULL;
	nodo_t* nodo = abb_buscar(arbol->raiz, &padre, arbol->func_cmp, clave, true);
	if (!nodo) return NULL;
	void* dato;
	if (nodo->izq == NULL || nodo->der == NULL) {
		dato = abb_borrar_0_1_hijos(arbol, clave, nodo, padre);
	} else {
		dato = abb_borrar_2_hijos(arbol, clave, nodo);
	}
	return dato;
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
	nodo_t* nodo = abb_buscar(arbol->raiz, NULL, arbol->func_cmp, clave, false);
	if (!nodo) return NULL;
	return nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
	return abb_buscar(arbol->raiz, NULL, arbol->func_cmp, clave, false);
}

size_t abb_cantidad(const abb_t *arbol) {
	return arbol->cantidad;
}

void abb_destruir(abb_t *arbol) {
	destruir_nodos(arbol->raiz, arbol->func_dest);
	free(arbol);
}


/* ******************************************************************
 *                    PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

bool _abb_in_order(nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra) {
	if (!nodo) return true;
	if (!_abb_in_order(nodo->izq, visitar, extra)) return false;
	if (!visitar(nodo->clave, nodo->dato, extra)) return false;
	if (!_abb_in_order(nodo->der, visitar, extra)) return false;
	return true;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
	_abb_in_order(arbol->raiz, visitar, extra);
}


/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
	if (!arbol) return NULL;
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (!iter) return NULL;
	pila_t* pila = pila_crear();
	if (!pila) return NULL;
	iter->pila = pila;
	
	if (arbol->raiz) {
		apilar(iter, arbol->raiz);
	}
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
	if (abb_iter_in_al_final(iter)) return false;
	nodo_t* actual = pila_desapilar(iter->pila);
	apilar(iter, actual->der);
	return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
	nodo_t* tope = pila_ver_tope(iter->pila);
	return abb_iter_in_al_final(iter) ? NULL : tope->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter) {
	pila_destruir(iter->pila);
	free(iter);
}
