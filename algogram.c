#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "algogram.h"
#include "usuario.h"
#include "post.h"
#include "hash.h"


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct algogram {
	hash_t* usuarios;
	hash_t* posts;
	char* usuario_loggeado;
	size_t id_usuario;
	size_t id_post;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* Devuelve una linea ingresada por entrada estandar, sin el ultimo caracter newline, 
 * en una variable en memoria dinamica.
 */
char* obtener_linea() {
	char* linea = NULL;
	size_t capacidad;
	if (getline(&linea, &capacidad, stdin) == -1) {
		free(linea);
		return NULL;
	}
	linea[strcspn(linea, "\n")] = 0;
	return linea;
}

// Es un wrapper de la primitiva usuario_destruir para utilizar en la creacion del hash de usuarios.
void usuario_destruir_wrapper(void* usuario) {
	usuario_destruir(usuario);
}

// Es un wrapper de la primitiva post_destruir para utilizar en la creacion del hash de posts.
void post_destruir_wrapper(void* post){
	post_destruir(post);
}

/* PRE: Recibe un AlgoGram previamente creado y un Post previamente creado.
 * POST: Devuelve true si el post se pudo guardar en el hash de posts, en caso contrario false.
 */
bool publicar_en_posts(algogram_t* algogram, post_t* post) {
	char id_post[post_ver_id(post) + 2];
	sprintf(id_post, "%zu", post_ver_id(post));
	if (!hash_guardar(algogram->posts, id_post, post)) {
		post_destruir(post);
		return false;
	}
	return true;
}

/* PRE: Recibe el ID del usuario al cual le pertenece el feed, y el ID del posteador.
 * POST: Devuelve la afinidad entre dichos usuarios.
 */
size_t calcular_afinidad(size_t id_usuario, size_t id_posteador) {
	return (size_t)abs((int)id_usuario - (int)id_posteador);
}

/* PRE: Recibe un AlgoGram previamente creado, un post previamente creado y el ID del posteador.
 * POST: Devuelve true si se pudo guardar el post en los feeds de los demas usuarios, en caso contrario devuelve false.
 */
bool publicar_en_usuarios(algogram_t* algogram, post_t* post, size_t id_posteador) {
	hash_iter_t* usuarios_iter = hash_iter_crear(algogram->usuarios);
	while (!hash_iter_al_final(usuarios_iter)) {
		const char* nombre = hash_iter_ver_actual(usuarios_iter);
		usuario_t* usuario = hash_obtener(algogram->usuarios, nombre);
		if (strcmp(nombre, algogram->usuario_loggeado) != 0) {
			size_t id_usuario = usuario_obtener_id(usuario);
			size_t afinidad = calcular_afinidad(id_usuario, id_posteador);
			if (!usuario_guardar_feed(usuario, post, afinidad)) {
				post_destruir(post);
				return false;
			}
		}
		hash_iter_avanzar(usuarios_iter);
	}
	hash_iter_destruir(usuarios_iter);
	return true;
}


/* ******************************************************************
 *                    PRIMITIVAS DE ALGOGRAM
 * *****************************************************************/

algogram_t* algogram_crear() {
	algogram_t* algogram = malloc(sizeof(algogram_t));
	if (!algogram) return NULL;
	hash_t* usuarios = hash_crear(usuario_destruir_wrapper);
	if (!usuarios) {
		free(algogram);
		return NULL;
	}
	algogram->usuarios = usuarios;
	hash_t* posts = hash_crear(post_destruir_wrapper);
	if (!posts) {
		free(algogram);
		free(usuarios);
		return NULL;
	}
	algogram->posts = posts;
	algogram->usuario_loggeado = NULL;
	algogram->id_usuario = 0;
	algogram->id_post = 0;
	return algogram;
}

bool algogram_agregar_usuario(algogram_t* algogram, const char* nombre_usuario) {
	usuario_t* usuario = usuario_crear(nombre_usuario, algogram->id_usuario); 
	if (!usuario) return false;
	if (!hash_guardar(algogram->usuarios, nombre_usuario, usuario)) {
		usuario_destruir(usuario);
		return false;
	}
	algogram->id_usuario++;
	return true;
}

bool algogram_login(algogram_t* algogram) {
	if (algogram->usuario_loggeado) {
		fprintf(stdout, "Error: Ya habia un usuario loggeado\n");
		return false;
	}
	size_t capacidad;
	if (getline(&algogram->usuario_loggeado, &capacidad, stdin) == -1) return false;
	algogram->usuario_loggeado[strcspn(algogram->usuario_loggeado, "\n")] = 0;
	if (!hash_pertenece(algogram->usuarios, algogram->usuario_loggeado)) {
		fprintf(stdout, "Error: usuario no existente\n");
		free(algogram->usuario_loggeado);
		algogram->usuario_loggeado = NULL;
		return false;
	}
	fprintf(stdout, "Hola %s\n", algogram->usuario_loggeado);
	return true;
}

bool algogram_logout(algogram_t* algogram) {
	if (!algogram->usuario_loggeado) {
		fprintf(stdout, "Error: no habia usuario loggeado\n");
		return false;
	}
	free(algogram->usuario_loggeado);
	algogram->usuario_loggeado = NULL;
	fprintf(stdout, "Adios\n");
	return true;
}

bool algogram_publicar_post(algogram_t* algogram) {
	if (!algogram->usuario_loggeado) { 
		fprintf(stdout, "Error: no habia usuario loggeado\n");
		return false;
	}
	char* texto = obtener_linea();
	post_t* post = post_crear(strdup(algogram->usuario_loggeado), texto, algogram->id_post);
	if (!post) {
		fprintf(stdout, "Error: no se pudo crear el post\n");	
		return false;
	}
	
	usuario_t* usuario_posteador = hash_obtener(algogram->usuarios, algogram->usuario_loggeado);
	size_t id_posteador = usuario_obtener_id(usuario_posteador);
	if (!publicar_en_posts(algogram, post)) return false;
	if (!publicar_en_usuarios(algogram, post, id_posteador)) return false;
	
	algogram->id_post++;
	fprintf(stdout, "Post publicado\n");
	return true;
}

bool algogram_ver_post(algogram_t* algogram) {
	if (!algogram->usuario_loggeado) {
		fprintf(stdout, "Usuario no loggeado o no hay mas posts para ver\n");
		return false;
	}
	usuario_t* usuario = hash_obtener(algogram->usuarios, algogram->usuario_loggeado);
	post_t* post = usuario_ver_post(usuario);
	if (!usuario || !post) {
		fprintf(stdout, "Usuario no loggeado o no hay mas posts para ver\n");
		return false;
	}
	fprintf(stdout, "Post ID %zu\n", post_ver_id(post));
	fprintf(stdout, "%s dijo: %s\n", post_ver_posteador(post), post_ver_texto(post));
	fprintf(stdout, "Likes: %zu\n", post_cantidad_likes(post));
	return true;
}

bool algogram_likear_post(algogram_t* algogram) {
	char* id_post = obtener_linea(); 
	post_t* post = hash_obtener(algogram->posts, id_post);
	if (!algogram->usuario_loggeado || !post) {
		fprintf(stdout, "Error: Usuario no loggeado o Post inexistente\n");
		free(id_post);
		return false;
	}
	if (!post_esta_likeado(post, algogram->usuario_loggeado)) {
		post_likear(post, algogram->usuario_loggeado);
	}
	free(id_post);
	fprintf(stdout, "Post likeado\n");
	return true;
}

bool algogram_ver_likes(algogram_t* algogram) {
	char* id_post = obtener_linea();
	post_t* post = hash_obtener(algogram->posts, id_post);
	if (!post || !post_cantidad_likes(post)) {
		fprintf(stdout, "Error: Post inexistente o sin likes\n");
		free(id_post);
		return false;
	}
	free(id_post);
	post_ver_likes(post);
	return true;
}

void algogram_destruir(algogram_t* algogram) {
	hash_destruir(algogram->usuarios);
	hash_destruir(algogram->posts);
	free(algogram->usuario_loggeado);
	free(algogram);
}
