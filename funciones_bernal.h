#ifndef FUNCIONES_BERNAL_H_INCLUDED
#define FUNCIONES_BERNAL_H_INCLUDED
#define ERR_PARAMETRO -6
#define ERR_NO_FILTRO -5
#define ERR_MEMORIA -4
#define ERR_PROFUNDIDAD -3
#define ERR_ARCHIVO_BMP -2
#define ERR_LECTURA -1
#define TODO_OK 0

#include "estructuras.h"

int concatenarVertical(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2, char * nombreArchivoBMP[]);
int rotarImagenIzquierda(t_pixel** imagen, t_metadata *metadata, char * nombreArchivoBMP[]);
int recortar(t_pixel ***imagen, t_metadata *metadata, float parametro, char * nombreArchivoBMP[]);
int achicar(t_pixel **imagen, t_metadata *metadata, unsigned int parametro, char * nombreArchivoBMP[]);
int escalaDeGrises(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[]);
int espejarVertical(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[]);

#endif // FUNCIONES_BERNAL_H_INCLUDED
