#include <stdlib.h>

#include "usuario.h"
#include "heap.h"


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct usuario {
	const char* nombre; 
	heap_t* feed;
	size_t id;
};

typedef struct post_afinidad {
	post_t* post;
	size_t afinidad;
} post_afinidad_t;


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* PRE: Recibe un post y su afinidad.
 * POST: Devuelve un struct post_afinidad que contiene dicho post y afinidad.
 */
post_afinidad_t* post_afinidad_crear(post_t* post, size_t afinidad) {
	post_afinidad_t* post_afinidad = malloc(sizeof(post_afinidad_t));
	if (!post_afinidad) return NULL;
	post_afinidad->post = post;
	post_afinidad->afinidad = afinidad;
	return post_afinidad;
}

/* PRE: Recibe un struct post_afinidad.
 * POST: Se destruyo el struct. 
 * (La funcion tiene un casteo para cumplir con la firma de la funcion de destruccion del heap.)
 */
void post_afinidad_destruir(void* post_afinidad) {
	post_afinidad_t* post_a_destruir= (post_afinidad_t*)post_afinidad;
	free(post_a_destruir);
}

/* PRE: Recibe dos posts a comparar.
 * POST: Luego de comparar los dos posts, devuelve:
 *   menor a 0  si  a < b
 *       0      si  a == b
 *   mayor a 0  si  a > b
 */
int afinidad(const void* a, const void* b) {
	post_afinidad_t* post_a = (post_afinidad_t*)a;
	post_afinidad_t* post_b = (post_afinidad_t*)b;
	if (post_a->afinidad > post_b->afinidad) return -1;
	if (post_a->afinidad == post_b->afinidad) {
		if (post_ver_id(post_a->post) > post_ver_id(post_b->post)) return -1;
		if (post_ver_id(post_a->post) == post_ver_id(post_b->post)) return 0;
		if (post_ver_id(post_a->post) < post_ver_id(post_b->post)) return 1;
	}
	return 1;
}

/* ******************************************************************
 *                    PRIMITIVAS DE USUARIO
 * *****************************************************************/

usuario_t* usuario_crear(const char* nombre, size_t id) {
	usuario_t* usuario = malloc(sizeof(usuario_t));
	if (!usuario) return NULL;
	heap_t* feed = heap_crear(afinidad);
	if (!feed) {
		free(usuario);
		return NULL;
	}
	usuario->feed = feed;
	usuario->nombre = nombre;
	usuario->id = id;
	return usuario;
}

bool usuario_guardar_feed(usuario_t* usuario, void* post, size_t afinidad) {
	post_afinidad_t* post_afinidad = post_afinidad_crear(post, afinidad);
	if (!heap_encolar(usuario->feed, post_afinidad)) return false;
	return true;
}

post_t* usuario_ver_post(usuario_t* usuario) {
	if (heap_esta_vacio(usuario->feed)) return NULL;
	post_afinidad_t* post_afinidad = heap_desencolar(usuario->feed);
	post_t* post = post_afinidad->post;
	post_afinidad_destruir(post_afinidad);
	return post;
}

size_t usuario_obtener_id(usuario_t* usuario) {
	return usuario->id;
}

void usuario_destruir(usuario_t* usuario) {
	heap_destruir(usuario->feed, post_afinidad_destruir);
	free(usuario);
}
