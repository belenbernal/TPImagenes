#ifndef FUNCIONES_FERNANDEZ_H_INCLUDED
#define FUNCIONES_FERNANDEZ_H_INCLUDED
#include "estructuras.h"

void obtenerArgumentos(char **nombreArchivoBMP, char *filtros[], int argc, char *argv[], int *numFiltros);
void mostrarArgumentos(char **nombreArchivoBMP, char *filtros[], int numFiltros);
t_pixel **leerImagenBMP(char *nombreArchivoBMP, t_metadata *header);
void leerEncabezado(FILE *archivo, t_metadata *metadata);
int rotarImagenDerecha(t_pixel** imagen, t_metadata *metadata);
void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz);

#endif // FUNCIONES_FERNANDEZ_H_INCLUDED
