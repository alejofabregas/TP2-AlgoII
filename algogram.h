#ifndef ALGOGRAM_H
#define ALGOGRAM_H

#include <stdbool.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct algogram algogram_t;


/* ******************************************************************
 *                    PRIMITIVAS DE ALGOGRAM
 * *****************************************************************/

// Crea la estructura AlgoGram sin usuarios.
algogram_t* algogram_crear();

/* PRE: Recibe un AlgoGram previamente creado y un nombre de usuario. 
 * POST: Devuelve true si pudo agregar el usuario a AlgoGram, en caso contrario false.
 */
bool algogram_agregar_usuario(algogram_t* algogram, const char* usuario);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Devuelve true si se pudo loggear el usuario, en caso contrario false.
 * Se puede loggear si no hay usuario loggeado y si el usuario se encuentra en el archivo de usuarios.
 */
bool algogram_login(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Devuelve true si se pudo realizar el logout, en caso de no haber un usuario loggeado devuelve false.
 */
bool algogram_logout(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Publica un post y lo agrega al feed de los demás usuarios y al hash de posts.
 */
bool algogram_publicar_post(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Devuelve true si se pudo mostrar el post, en caso de no haber más posts por ver o no haber 
 * usuario loggeado devuelve false.
 */
bool algogram_ver_post(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Devuelve true si se likeo el post correspondiente al ID que se recibe por consola, en caso de 
 * no existir el post de dicho ID o no haber usuario loggeado devuelve false.
 */
bool algogram_likear_post(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado
 * POST: Devuelve true y muestra los likes del post correspondiente al ID que recibe por consola,
 * o false si no existe el post o no tiene likes.
 */
bool algogram_ver_likes(algogram_t* algogram);

/* PRE: Recibe un AlgoGram previamente creado.
 * POST: Se destruyo el AlgoGram.
 */
void algogram_destruir(algogram_t* algogram);


#endif  // ALGOGRAM_H
