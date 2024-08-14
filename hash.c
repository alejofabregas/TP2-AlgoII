#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdlib.h>
#include <string.h>

#define CAPACIDAD_INICIAL 5
#define FACTOR_CARGA_MAX 0.65
#define FACTOR_CARGA_MIN 0.1
#define FACTOR_AGRANDAMIENTO 2
#define FACTOR_ACHICAMIENTO 3
#define ERROR -1

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef enum { VACIO, OCUPADO, BORRADO } estado_t;

typedef struct celda {
	char* clave;
	void* dato;
	estado_t estado;
} celda_t;

struct hash {
	size_t cantidad;
	size_t cantidad_borrados;
	size_t capacidad;
	celda_t* tabla;
	void (*func_dest)(void*);
};

struct hash_iter {
	const hash_t* hash;
	size_t pos;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

// RS Hash de Robert Sedgewick.
// https://www.programmingalgorithms.com/algorithm/rs-hash/c/
unsigned long hashing(const char* str) {
	unsigned long length = (unsigned long)strlen(str);
	unsigned long b = 378551;
	unsigned long a = 63689;
	unsigned long hash = 0;
	unsigned long i = 0;
	for (i = 0; i < length; str++, i++) {
		hash = hash * a + (*str);
		a = a * b;
	}
	return hash;
}

/* PRE: Recibe la capacidad de la tabla.
 * POST: Devuelve una tabla inicializada o NULL si no se pudo crear.
 */
celda_t* hash_crear_tabla(size_t capacidad) {
	celda_t* tabla = calloc(capacidad, sizeof(celda_t));
	if (!tabla) return NULL;
	return tabla;
}

/* PRE: Recibe un dato, una clave y una funcion de destruir dato o NULL.
 * POST: Destruye una celda.
 */
void hash_celda_destruir(void* dato, char* clave, hash_destruir_dato_t destruir_dato) {
	free(clave);
	if (destruir_dato != NULL) destruir_dato(dato);
}

/* PRE: Recibe el hash, una clave, un dato y una posicion donde se quiere guardar.
 * POST: Guarda el dato en la celda y aumenta la cantidad del hash.
 */
void hash_celda_guardar_dato(hash_t* hash, const char* clave, void* dato, size_t pos) {
	hash->tabla[pos].estado = OCUPADO;
	hash->tabla[pos].dato = dato;
	hash->tabla[pos].clave = strdup(clave);
	hash->cantidad++;
}

/* PRE: Recibe el hash y una posicion donde se quiere borrar.
 * POST: Borra el dato de la celda, resta la cantidad del hash y aumenta la
 * cantidad de borrados. Devuelve el dato borrado.
 */
void* hash_celda_borrar_dato(hash_t* hash, size_t pos) {
	void* dato = hash->tabla[pos].dato;
	hash->tabla[pos].estado = BORRADO;
	free(hash->tabla[pos].clave);
	hash->cantidad_borrados++;
	hash->cantidad--;
	return dato;
}

/* PRE: El hash fue creado.
 * POST: Devuelve FALSE si no se pudo redimensionar, en otro caso TRUE.
 */
bool hash_redimensionar(hash_t* hash, size_t capacidad_nueva) {
	celda_t* tabla_nueva = hash_crear_tabla(capacidad_nueva);
	if (!tabla_nueva) return false;

	celda_t* tabla_vieja = hash->tabla;
	size_t capacidad_vieja = hash->capacidad;

	hash->tabla = tabla_nueva;
	hash->cantidad = 0;
	hash->cantidad_borrados = 0;
	hash->capacidad = capacidad_nueva;

	for (size_t i = 0; i < capacidad_vieja; i++) {
		if (tabla_vieja[i].estado == OCUPADO) {
			if (!hash_guardar(hash,tabla_vieja[i].clave, tabla_vieja[i].dato)) return false;
			free(tabla_vieja[i].clave);
		}
	}
	free(tabla_vieja);
	return true;
}

/* PRE: Recibe una clave y un hash.
 * POST: Devuelve la posicion de la clave si se encontro en el hash, si no se 
 * encontro devuelve -1 (ERROR). Con el parametro buscar_vacio se puede indicar
 * a la funcion que busque una posicion vacia.
 */
size_t hash_buscar(const hash_t* hash, const char* clave, bool buscar_vacio) {
	size_t pos = hashing(clave) % hash->capacidad;
	while (hash->tabla[pos].estado != VACIO) {
		if (hash->tabla[pos].estado == OCUPADO && strcmp(hash->tabla[pos].clave, clave) == 0) return pos;
		if (pos == hash->capacidad - 1) pos = 0;
		else pos++;
	}
	return buscar_vacio ? pos : ERROR;
}

/* PRE: Recibe un iterador.
 * POST: Devuelve TRUE si encontro una posicion valida y la asigna, sino FALSE.
 */
bool posicion_valida(hash_iter_t* iter) {
	if (hash_cantidad(iter->hash) == 0) return true;
	while (iter->pos < iter->hash->capacidad) {
		if (iter->hash->tabla[iter->pos].estado == OCUPADO) return true;
		iter->pos++;
	}
	return false;
}


/* ******************************************************************
 *                    PRIMITIVAS DEL HASH 
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;

	hash->cantidad = 0;
	hash->capacidad = CAPACIDAD_INICIAL;
	hash->cantidad_borrados = 0;
	hash->func_dest = destruir_dato;
	
	hash->tabla = hash_crear_tabla(hash->capacidad);
	if (!hash->tabla) return NULL;
	
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	if ((double)(hash->cantidad + hash->cantidad_borrados) / (double)hash->capacidad >= FACTOR_CARGA_MAX) {
		if (!hash_redimensionar(hash, hash->capacidad * FACTOR_AGRANDAMIENTO)) return false;
	}
	size_t pos = hash_buscar(hash, clave, true);
	if (hash->tabla[pos].estado == OCUPADO) {
		if (hash->func_dest) hash->func_dest(hash->tabla[pos].dato);
		hash->tabla[pos].dato = dato;
		return true;
	}
	hash_celda_guardar_dato(hash, clave, dato, pos);
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {
	size_t pos = hash_buscar(hash, clave, false);
	if (pos == ERROR) return NULL;
	void* dato = hash_celda_borrar_dato(hash, pos);
	
	if ((double)(hash->cantidad + hash->cantidad_borrados) / (double)hash->capacidad <= FACTOR_CARGA_MIN) {
		if (!hash_redimensionar(hash, hash->capacidad / FACTOR_ACHICAMIENTO)) return false;
	}
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave) {
	size_t pos = hash_buscar(hash, clave, false);
	return pos == ERROR ? NULL : hash->tabla[pos].dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	return hash_buscar(hash, clave, false) == ERROR ? false : true;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void hash_destruir(hash_t *hash) {
	for (size_t i = 0; i < hash->capacidad; i++) {
		if (hash->tabla[i].estado == OCUPADO) {
			hash_celda_destruir(hash->tabla[i].dato, hash->tabla[i].clave, hash->func_dest);
		}
	}
	free(hash->tabla);
	free(hash);
}


/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR HASH
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter) return NULL;
	iter->hash = hash;
	iter->pos = 0;
	if (!posicion_valida(iter)) return NULL;
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
	iter->pos++;
	return hash_iter_al_final(iter) ? false : posicion_valida(iter);
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	return hash_iter_al_final(iter) ? NULL : iter->hash->tabla[iter->pos].clave;
}
	
bool hash_iter_al_final(const hash_iter_t *iter) {
	return iter->pos >= iter->hash->capacidad || hash_cantidad(iter->hash) == 0; 
}
void hash_iter_destruir(hash_iter_t *iter) {
	free(iter);
}

