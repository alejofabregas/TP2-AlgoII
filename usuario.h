#ifndef USUARIO_H
#define USUARIO_H

#include <stdlib.h>

#include "post.h"


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct usuario usuario_t;


/* ******************************************************************
 *                    PRIMITIVAS DE USUARIO
 * *****************************************************************/

/* PRE: Recibe el nombre del usuario y su ID.
 * POST: Devuelve el usuario que fue creado con dichas caracteristicas.
 */
usuario_t* usuario_crear(const char* nombre, size_t id);

/* PRE: Recibe un usuario previamente creado.
 * POST: Devuelve el post que sigue en el feed de ese usuario o NULL si no hay más para ver.
 */
post_t* usuario_ver_post(usuario_t* usuario);

/* PRE: Recibe un usuario previamente creado, un post y la afinidad que tiene ese usuario con el posteador.
 * POST: Devuelve true si se pudo guardar el post en el feed del usuario, en caso contrario false.
 */
bool usuario_guardar_feed(usuario_t* usuario, void* post, size_t afinidad);

/* PRE: Recibe un usuario previamente creado.
 * POST: Devuelve el ID del post.
 */
size_t usuario_obtener_id(usuario_t* usuario);

/* PRE: Recibe un usuario previamente creado.
 * POST: Destruye el usuario.
 */
void usuario_destruir(usuario_t* usuario);

#endif  // USUARIO_H
