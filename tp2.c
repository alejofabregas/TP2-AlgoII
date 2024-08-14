#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "algogram.h"

#define PARAM_ARCHIVO 1


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

/* PRE: Recibe la cantidad de parametros del main.
 * POST: Devuelve true si la cantidad de parametros es válida, false si es invalida.
 */
bool validar_params(int argc) {
	if (argc != 2) {
		fprintf(stdout, "Error: parametros invalidos");
		return false;
	}
	return true;
}

/* PRE: Recibe el nombre del archivo a abrir.
 * POST: Devuelve el archivo abierto, si no se pudo abrir imprime un mensaje de error.
 */
FILE* abrir_archivo(char* nom_archivo) {
	FILE* archivo = fopen(nom_archivo, "r");
	if (!archivo) {
		fprintf(stderr, "%s", "Error: no se pudo abrir el archivo de usuarios\n");
	}
	return archivo;
}

/* PRE: Recibe un AlgoGram y un archivo de texto con usuarios.
 * POST: Todos los usuarios del archivo fueron agregados a AlgoGram.
 */
void obtener_usuarios(algogram_t* algogram, FILE* archivo) {
	char* linea = NULL;
	size_t capacidad;
	while (getline(&linea, &capacidad, archivo) != EOF) {
		linea[strcspn(linea, "\n")] = 0;
		if (!algogram_agregar_usuario(algogram, linea)) fprintf(stdout, "Error: no se pudo obtener el usuario\n");
	}
	free(linea);
}

/* PRE: Recibe un AlgoGram.
 * POST: Se ejecutaron todos los comandos válidos de AlgoGram que se hayan ingresado por entrada estandar.
 */
void recibir_comandos(algogram_t* algogram) {
	char* linea = NULL;
	size_t capacidad;
	while (getline(&linea, &capacidad, stdin) != EOF) {
		if (strcmp(linea, "login\n") == 0) {
			algogram_login(algogram);
		} else if (strcmp(linea, "logout\n") == 0) {
			algogram_logout(algogram);
		} else if (strcmp(linea, "publicar\n") == 0) {
			algogram_publicar_post(algogram);
		} else if (strcmp(linea, "ver_siguiente_feed\n") == 0) {
			algogram_ver_post(algogram);
		} else if (strcmp(linea, "likear_post\n") == 0) {
			algogram_likear_post(algogram);
		} else if (strcmp(linea, "mostrar_likes\n") == 0) {
			algogram_ver_likes(algogram);
		}
	}
	free(linea);
}


/* *****************************************************************
 *                    			MAIN
 * *****************************************************************/

int main(int argc, char* argv[]) {
	
	/* Validacion de parametros */
	if (!validar_params(argc)) {
		return -1;
	}
	/* Creo la estructura de AlgoGram */
	algogram_t* algogram = algogram_crear();
	if (!algogram) {
		fprintf(stdout, "Error: no se pudo iniciar AlgoGram");
		return -1;
	}
	/* Obtengo los usuarios del archivo de texto */
	FILE* archivo = abrir_archivo(argv[PARAM_ARCHIVO]);
	if (!archivo) {
		return -1;
	}
	obtener_usuarios(algogram, archivo);
	fclose(archivo);
	
	/* Espero comandos por consola */
	recibir_comandos(algogram);
	
	/* Destruyo la estructura de AlgoGram */
	algogram_destruir(algogram);
	
	return 0;
}
