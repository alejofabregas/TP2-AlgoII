#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "post.h"
#include "abb.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct post {
	size_t id;
	char* posteador;
	char* texto;
	abb_t* likes;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* PRE: Recibe el nombre del usuario a imprimir. Como es la funcion visitar del iterador in order
 * del ABB, se desprecian los otros dos parametros.
 * POST: Se imprimio el usuario que le dio like al post, pasado por parametro.
 */
bool imprimir_likes(const char* usuario, void* dato, void* extra) {
	fprintf(stdout, "\t%s\n", usuario);
	return true;
}


/* ******************************************************************
 *                    PRIMITIVAS DE POST
 * *****************************************************************/

post_t* post_crear(char* nombre_usuario, char* texto, size_t id) {
	post_t* post = malloc(sizeof(post_t));
	if (!post) return NULL;
	abb_t* likes = abb_crear(strcmp, NULL);
	if (!likes) {
		free(post);
		return NULL;
	}
	post->likes = likes;
	post->id = id;
	post->posteador = nombre_usuario;
	post->texto = texto;
	return post;
}

size_t post_ver_id(post_t* post) {
	return post->id;
}

char* post_ver_posteador(post_t* post) {
	return post->posteador;
}

char* post_ver_texto(post_t* post) {
	return post->texto;
}

bool post_likear(post_t* post, char* usuario) {
	return abb_guardar(post->likes, usuario, NULL);
}

bool post_esta_likeado(post_t* post, char* usuario) {
	return abb_pertenece(post->likes, usuario);
}

size_t post_cantidad_likes(post_t* post) {
	return abb_cantidad(post->likes);
}

void post_ver_likes(post_t* post) {
	fprintf(stdout, "El post tiene %zu likes:\n", post_cantidad_likes(post));
	abb_in_order(post->likes, imprimir_likes, NULL);
}

void post_destruir(post_t* post) {
	abb_destruir(post->likes);
	free(post->texto);
	free(post->posteador);
	free(post);
}
