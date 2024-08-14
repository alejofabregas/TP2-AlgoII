#ifndef POST_H
#define POST_H

#include <stdlib.h>
#include <stdbool.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct post post_t;

/* ******************************************************************
 *                    PRIMITIVAS DE POST
 * *****************************************************************/

/* PRE: Recibe el nombre de un usuario, el texto del post y el ID que va a tener el post.
 * POST: Devuelve el post que fue creado con dichos parametros.
 */
post_t* post_crear(char* nombre_usuario, char* texto, size_t id);

/* PRE: Recibe un post previamente creado.
 * POST: Devuelve el ID del post.
 */
size_t post_ver_id(post_t* post);

/* PRE: Recibe un post previamente creado.
 * POST: Devuelve el nombre del usuario que posteo ese post.
 */
char* post_ver_posteador(post_t* post);

/* PRE: Recibe un post previamente creado.
 * POST: Devuelve el contenido del post.
 */
char* post_ver_texto(post_t* post);

/* PRE: Recibe un post previamente creado y un nombre de usuario.
 * POST: Devuelve true si ese usuario pudo likear el post, o false en caso contrario.
 */
bool post_likear(post_t* post, char* usuario);

/* PRE: Recibe un post previamente creado y un nombre de usuario.
 * POST: Devuelve true si el post esta likeado por ese usuario, en caso contrario false.
 */
bool post_esta_likeado(post_t* post, char* usuario);

/* PRE: Recibe un post previamente creado.
 * POST: Devuelve la cantidad de likes que tiene el post.
 */
size_t post_cantidad_likes(post_t* post);

/* PRE: Recibe un post previamente creado.
 * POST: Imprime por pantalla los usuarios que likearon ese post, en orden alfabetico.
 */
void post_ver_likes(post_t* post);

/* PRE: Recibe un post previamente creado.
 * POST: Destruye el post.
 */
void post_destruir(post_t* post);

#endif  // POST_H
